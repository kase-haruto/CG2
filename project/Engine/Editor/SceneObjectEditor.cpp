#include "SceneObjectEditor.h"
/* ========================================================================
/*		include space
/* ===================================================================== */
// engine
#include <Engine/Objects/3D/Actor/SceneObject.h>
#include <Engine/graphics/Camera/Manager/CameraManager.h>
#include <Engine/Foundation/Utility/Func/MyFunc.h>

// externals
#include <externals/imgui/imgui.h>
#include "externals/imgui/ImGuizmo.h"

SceneObjectEditor::SceneObjectEditor(const std::string& name):BaseEditor(name){
}

void SceneObjectEditor::ShowImGuiInterface(){
	if (!sceneObject_) return;
	ShowGuizmo();
	sceneObject_->ShowGui();

}
void RowToColumnArray(const Matrix4x4& m, float out[16]){
	// 回転スケール 3×3 を転置
	out[0] = m.m[0][0]; out[1] = m.m[1][0]; out[2] = m.m[2][0]; out[3] = 0.0f;
	out[4] = m.m[0][1]; out[5] = m.m[1][1]; out[6] = m.m[2][1]; out[7] = 0.0f;
	out[8] = m.m[0][2]; out[9] = m.m[1][2]; out[10] = m.m[2][2]; out[11] = 0.0f;

	// 平行移動はそのまま 12–14
	out[12] = m.m[3][0];
	out[13] = m.m[3][1];
	out[14] = m.m[3][2];
	out[15] = 1.0f;
}

// column-major[16] → DirectX(row-major)
Matrix4x4 ColumnArrayToRow(const float in_[16]){
	Matrix4x4 m;
	// 3×3 転置（＝再転置で元に戻る）
	m.m[0][0] = in_[0];  m.m[0][1] = in_[4];  m.m[0][2] = in_[8];  m.m[0][3] = 0.0f;
	m.m[1][0] = in_[1];  m.m[1][1] = in_[5];  m.m[1][2] = in_[9];  m.m[1][3] = 0.0f;
	m.m[2][0] = in_[2];  m.m[2][1] = in_[6];  m.m[2][2] = in_[10]; m.m[2][3] = 0.0f;

	// 平行移動
	m.m[3][0] = in_[12];
	m.m[3][1] = in_[13];
	m.m[3][2] = in_[14];
	m.m[3][3] = 1.0f;
	return m;
}
//====================================================================//
//  SceneObjectEditor::ShowGuizmo
//====================================================================//
void SceneObjectEditor::ShowGuizmo(){
	// -----------------------------------------------------------------
	// 0. ガード
	// -----------------------------------------------------------------
	if (!sceneObject_ || sceneObject_->GetObjectType() != ObjectType::GameObject) return;
	WorldTransform& wt = sceneObject_->GetWorldTransform();

	// -----------------------------------------------------------------
	// 1. カメラ行列（column-major）
	// -----------------------------------------------------------------
	auto* camMgr = CameraManager::GetInstance();
	if (!camMgr){ ImGui::Text("CameraManager not found!"); return; }
	auto* cam = camMgr->GetActiveCamera();
	if (!cam){ ImGui::Text("No Active Camera found!");  return; }

	float viewCM[16]; Matrix4x4::Transpose(cam->GetViewMatrix()).CopyToArray(viewCM);
	float projCM[16]; Matrix4x4::Transpose(cam->GetProjectionMatrix()).CopyToArray(projCM);

	// -----------------------------------------------------------------
	// 2. 編集対象のワールド行列（column-major）
	// -----------------------------------------------------------------
	float worldCM[16];
	Matrix4x4::Transpose(wt.matrix.world).CopyToArray(worldCM);

	// -----------------------------------------------------------------
	// 3. ImGuizmo 操作
	// -----------------------------------------------------------------
	static ImGuizmo::OPERATION op = ImGuizmo::TRANSLATE;
	static ImGuizmo::MODE      mode = ImGuizmo::WORLD;

	ImGuizmo::Manipulate(viewCM, projCM, op, mode, worldCM);

	// ------------------------------------------------------------
	// 4. 変更を S / R / T へ反映
	// ------------------------------------------------------------
	if (ImGuizmo::IsUsing()){
		// column → row 行列
		Matrix4x4 worldEditedRow = ColumnArrayToRow(worldCM);

		// 親があるならローカルへ戻す
		Matrix4x4 localEditedRow = wt.parent
			? Matrix4x4::Inverse(wt.parent->matrix.world) * worldEditedRow
			: worldEditedRow;

		// 分解のため再び column 配列に
		float localCM[16];
		RowToColumnArray(localEditedRow, localCM);

		float pos[3], rotDeg[3], scl[3];
		ImGuizmo::DecomposeMatrixToComponents(localCM, pos, rotDeg, scl);

		// 4-D) 書き戻し
		wt.translation = {pos[0], pos[1], pos[2]};
		wt.scale = {scl[0], scl[1], scl[2]};

		constexpr float kRad = 3.14159265358979323846f / 180.0f;
		Vector3 eulerRotate = {rotDeg[0] * kRad, rotDeg[1] * kRad, rotDeg[2] * kRad};
		wt.rotation = Quaternion::EulerToQuaternion(eulerRotate);

	}

	// -----------------------------------------------------------------
	// 5. UI：モード切替
	// -----------------------------------------------------------------
	if (ImGui::RadioButton("Translate", op == ImGuizmo::TRANSLATE)) op = ImGuizmo::TRANSLATE;
	ImGui::SameLine();
	if (ImGui::RadioButton("Rotate", op == ImGuizmo::ROTATE)) op = ImGuizmo::ROTATE;
	ImGui::SameLine();
	if (ImGui::RadioButton("Scale", op == ImGuizmo::SCALE)) op = ImGuizmo::SCALE;
}
