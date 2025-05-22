#include <Engine/System/Command/EditorCommand/GuiCommand/ImGuiHelper/GuiCmd.h>
/* ========================================================================
/*		include space
/* ===================================================================== */
// engine
#include <Engine/System/Command/EditorCommand/GuiCommand/ImGuiHelper/GuiCmdInternal.h>

// math
#include <Engine/Foundation/Math/Vector2.h>
#include <Engine/Foundation/Math/Vector3.h>
#include <Engine/Foundation/Math/Vector4.h>


/* ==========================================================================================================
/*			DragFloat
/* ======================================================================================================== */
#pragma region DragFloat
bool GuiCmd::DragFloat(const char* label, float& value, float speed, float min, float max){
	static GuiCmdInternal::GuiCmdSetValueComputer<float> computer;
	float temp = value;
	bool changed = ImGui::DragFloat(label, &temp, speed, min, max);
	value = temp;

	// マウスが押された 検知開始
	if (ImGui::IsItemActivated()) computer.Begin(value);
	// マウスが離れた
	if (ImGui::IsItemDeactivatedAfterEdit()){
		auto cmd = computer.End(value, [&value] (const float& v){ value = v; });
		//マウスが押された位置から動いていたらコマンドを発行する
		if (cmd) CommandManager::GetInstance()->Execute(std::move(cmd));
	}
	return changed;
}

bool GuiCmd::DragFloat2(const char* label, Vector2& value, float speed, float min, float max){
	static GuiCmdInternal::GuiCmdSetValueComputer<Vector2> computer;
	Vector2 temp = value;
	bool changed = ImGui::DragFloat2(label, &temp.x, speed, min, max);
	value = temp;
	// マウスが押された 検知開始
	if (ImGui::IsItemActivated()) computer.Begin(value);
	// マウスが離れた
	if (ImGui::IsItemDeactivatedAfterEdit()){
		auto cmd = computer.End(value, [&value] (const Vector2& v){ value = v; });
		//マウスが押された位置から動いていたらコマンドを発行する
		if (cmd) CommandManager::GetInstance()->Execute(std::move(cmd));
	}
	return changed;
}

bool GuiCmd::DragFloat3(const char* label, Vector3& value, float speed, float min, float max){
	static GuiCmdInternal::GuiCmdSetValueComputer<Vector3> computer;
	Vector3 temp = value;
	bool changed = ImGui::DragFloat3(label, &temp.x, speed, min, max);
	value = temp;

	// マウスが押された 検知開始
	if (ImGui::IsItemActivated()) computer.Begin(value);

	// マウスが離れた
	if (ImGui::IsItemDeactivatedAfterEdit()){
		auto cmd = computer.End(value, [&value] (const Vector3& v){ value = v; });
		//マウスが押された位置から動いていたらコマンドを発行する
		if (cmd) CommandManager::GetInstance()->Execute(std::move(cmd));
	}
	return changed;
}

bool GuiCmd::DragFloat4(const char* label, Vector4& value, float speed, float min, float max){
	static GuiCmdInternal::GuiCmdSetValueComputer<Vector4> computer;
	Vector4 temp = value;
	bool changed = ImGui::DragFloat4(label, &temp.x, speed, min, max);
	value = temp;
	// マウスが押された 検知開始
	if (ImGui::IsItemActivated()) computer.Begin(value);
	// マウスが離れた
	if (ImGui::IsItemDeactivatedAfterEdit()){
		auto cmd = computer.End(value, [&value] (const Vector4& v){ value = v; });
		//マウスが押された位置から動いていたらコマンドを発行する
		if (cmd) CommandManager::GetInstance()->Execute(std::move(cmd));
	}
	return changed;
}
#pragma endregion

/* ==========================================================================================================
/*			SliderFloat
/* ======================================================================================================== */
#pragma region SliderFloat
bool GuiCmd::SliderFloat(const char* label, float& value, float min, float max){
	static GuiCmdInternal::GuiCmdSetValueComputer<float> computer;
	float temp = value;
	bool changed = ImGui::SliderFloat(label, &temp, min, max);
	value = temp;
	// マウスが押された 検知開始
	if (ImGui::IsItemActivated()) computer.Begin(value);
	// マウスが離れた
	if (ImGui::IsItemDeactivatedAfterEdit()){
		auto cmd = computer.End(value, [&value] (const float& v){ value = v; });
		//マウスが押された位置から動いていたらコマンドを発行する
		if (cmd) CommandManager::GetInstance()->Execute(std::move(cmd));
	}
	return changed;
}

bool GuiCmd::SliderFloat2(const char* label, Vector2& value, float min, float max){
	static GuiCmdInternal::GuiCmdSetValueComputer<Vector2> computer;
	Vector2 temp = value;
	bool changed = ImGui::SliderFloat2(label, &temp.x, min, max);
	value = temp;
	// マウスが押された 検知開始
	if (ImGui::IsItemActivated()) computer.Begin(value);
	// マウスが離れた
	if (ImGui::IsItemDeactivatedAfterEdit()){
		auto cmd = computer.End(value, [&value] (const Vector2& v){ value = v; });
		//マウスが押された位置から動いていたらコマンドを発行する
		if (cmd) CommandManager::GetInstance()->Execute(std::move(cmd));
	}
	return changed;
}

bool GuiCmd::SliderFloat3(const char* label, Vector3& value, float min, float max){
	static GuiCmdInternal::GuiCmdSetValueComputer<Vector3> computer;
	Vector3 temp = value;
	bool changed = ImGui::SliderFloat3(label, &temp.x, min, max);
	value = temp;
	// マウスが押された 検知開始
	if (ImGui::IsItemActivated()) computer.Begin(value);
	// マウスが離れた
	if (ImGui::IsItemDeactivatedAfterEdit()){
		auto cmd = computer.End(value, [&value] (const Vector3& v){ value = v; });
		//マウスが押された位置から動いていたらコマンドを発行する
		if (cmd) CommandManager::GetInstance()->Execute(std::move(cmd));
	}
	return changed;
}

bool GuiCmd::SliderFloat4(const char* label, Vector4& value, float min, float max){
	static GuiCmdInternal::GuiCmdSetValueComputer<Vector4> computer;
	Vector4 temp = value;
	bool changed = ImGui::SliderFloat3(label, &temp.x, min, max);
	value = temp;
	// マウスが押された 検知開始
	if (ImGui::IsItemActivated()) computer.Begin(value);
	// マウスが離れた
	if (ImGui::IsItemDeactivatedAfterEdit()){
		auto cmd = computer.End(value, [&value] (const Vector4& v){ value = v; });
		//マウスが押された位置から動いていたらコマンドを発行する
		if (cmd) CommandManager::GetInstance()->Execute(std::move(cmd));
	}
	return changed;
}
#pragma endregion

/* ==========================================================================================================
/*			ColorEdit
/* ======================================================================================================== */
#pragma region ColorEdit
bool GuiCmd::ColorEdit4(const char* label, Vector4& value, ImGuiColorEditFlags flags){
	static GuiCmdInternal::GuiCmdSetValueComputer<Vector4> computer;

	Vector4 temp = value;
	bool changed = ImGui::ColorEdit4(label, &temp.x, flags);

	// マウスが押された 検知開始
	if (ImGui::IsItemActivated()){
		computer.Begin(value);
	}

	value = temp;
	// マウスが離れた
	if (ImGui::IsItemDeactivatedAfterEdit()){
		auto cmd = computer.End(value, [&value] (const Vector4& v){ value = v; });
		//マウスが押された位置から動いていたらコマンドを発行する
		if (cmd) CommandManager::GetInstance()->Execute(std::move(cmd));
	}
	return changed;
}

#pragma endregion

/* ==========================================================================================================
/*			checkBox
/* ======================================================================================================== */
#pragma region CheckBox
bool GuiCmd::CheckBox(const char* label, bool& value){
	static GuiCmdInternal::GuiCmdSetValueComputer<bool> computer;
	bool temp = value;
	bool changed = ImGui::Checkbox(label, &temp);
	value = temp;

	// マウスが押された 検知開始
	if (ImGui::IsItemActivated()) computer.Begin(value);
	// マウスが離れた
	if (ImGui::IsItemDeactivatedAfterEdit()){
		auto cmd = computer.End(value, [&value] (const bool& v){ value = v; });
		//マウスが押された位置から動いていたらコマンドを発行する
		if (cmd) CommandManager::GetInstance()->Execute(std::move(cmd));
	}
	return changed;
}
#pragma endregion