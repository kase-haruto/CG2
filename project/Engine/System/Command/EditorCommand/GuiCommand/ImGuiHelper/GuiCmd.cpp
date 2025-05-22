#include <Engine/System/Command/EditorCommand/GuiCommand/ImGuiHelper/GuiCmd.h>

bool GuiCmd::DragFloat3(const char* label, Vector3& value, float speed, float min, float max){
	static Vector3 beforeValue;
	static bool dragging = false;

	Vector3 tempValue = value;
	bool changed = ImGui::DragFloat3(label, &tempValue.x, speed, min, max);

	// 値は即時反映（リアルタイムに見た目更新）
	value = tempValue;

	// ドラッグ開始時に値を記録
	if (ImGui::IsItemActivated() && !dragging){
		beforeValue = value;
		dragging = true;
	}

	// ドラッグ終了時に差があればコマンド化
	if (ImGui::IsItemDeactivatedAfterEdit() && dragging){
		dragging = false;

		auto IsNotEqual = [] (const Vector3& a, const Vector3& b){
			const float epsilon = 0.0001f;
			return std::fabs(a.x - b.x) > epsilon ||
				std::fabs(a.y - b.y) > epsilon ||
				std::fabs(a.z - b.z) > epsilon;
			};

		if (IsNotEqual(beforeValue, value)){
			auto cmd = std::make_unique<SetValueCommand<Vector3>>(beforeValue, value, [&value] (const Vector3& v){
				value = v;
																  });
			CommandManager::GetInstance()->Execute(std::move(cmd));
		}
	}

	return changed;
}
