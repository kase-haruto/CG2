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
#include <externals/imgui/ImGuizmo.h>

SceneObjectEditor::SceneObjectEditor() {
	editorName_ = "SceneObjectEditor";
}

void SceneObjectEditor::ShowImGuiInterface() {
	if (!sceneObject_) return;
	ShowGuizmo();
	sceneObject_->ShowGui();

}

void SceneObjectEditor::ShowGuizmo() {
	if (sceneObject_->GetObjectType() != ObjectType::GameObject)return;

	WorldTransform& worldTransform = sceneObject_->GetWorldTransform();

	// ==== 1. カメラ取得 ====
	CameraManager* camMgr = CameraManager::GetInstance();
	if (!camMgr) { ImGui::Text("CameraManager not found!"); return; }
	BaseCamera* camera = camMgr->GetActiveCamera();
	if (!camera) { ImGui::Text("No Active Camera found!"); return; }

	// ==== 2. カメラ行列 ====
	Matrix4x4 viewMat = camera->GetViewMatrix();
	Matrix4x4 projMat = camera->GetProjectionMatrix();
	float view[16]; memcpy(view, &viewMat, sizeof(view));
	float proj[16]; memcpy(proj, &projMat, sizeof(proj));

	// ==== 3. モデル行列 ====
	Matrix4x4 modelRM = MakeAffineMatrix(worldTransform.scale,
										 worldTransform.eulerRotation,
										 worldTransform.translation);
	if (worldTransform.parent) {
		auto parentMat = MakeAffineMatrix(worldTransform.parent->scale,
										  worldTransform.parent->eulerRotation,
										  worldTransform.parent->translation);
		modelRM = Matrix4x4::Multiply(parentMat, modelRM);
	}
	float model[16]; memcpy(model, &modelRM, sizeof(model));

	// ==== 4. ギズモ ====
	static ImGuizmo::OPERATION operation = ImGuizmo::TRANSLATE;
	static ImGuizmo::MODE gizmoMode = ImGuizmo::LOCAL;

	ImGuizmo::Manipulate(view, proj, operation, gizmoMode, model);

	// ==== 5. 編集結果を反映 ====
	if (ImGuizmo::IsUsing()) {
		Matrix4x4 updated; memcpy(&updated, model, sizeof(updated));
		if (worldTransform.parent) {
			auto parentMat = MakeAffineMatrix(worldTransform.parent->scale,
											  worldTransform.parent->eulerRotation,
											  worldTransform.parent->translation);
			auto parentInv = Matrix4x4::Inverse(parentMat);
			updated = Matrix4x4::Multiply(updated, parentInv);
		}

		Vector3 s, r, t;
		DecomposeMatrix(updated, s, r, t);

		worldTransform.scale = s;
		worldTransform.eulerRotation = r;
		worldTransform.translation = t;
	}

	// ==== 6. ギズモモード選択 ====
	if (ImGui::RadioButton("Translate", operation == ImGuizmo::TRANSLATE)) operation = ImGuizmo::TRANSLATE;
	ImGui::SameLine();
	if (ImGui::RadioButton("Rotate", operation == ImGuizmo::ROTATE)) operation = ImGuizmo::ROTATE;
	ImGui::SameLine();
	if (ImGui::RadioButton("Scale", operation == ImGuizmo::SCALE)) operation = ImGuizmo::SCALE;


}
