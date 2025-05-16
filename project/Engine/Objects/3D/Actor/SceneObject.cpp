#include "SceneObject.h"
#include "SceneObjectManager.h"
#include <Engine/graphics/Camera/Manager/CameraManager.h>
#include <Engine/Foundation/Utility/Func/MyFunc.h>

#include <externals/imgui/imgui.h>
#include <externals/imgui/ImGuizmo.h>

SceneObject::~SceneObject(){
}

SceneObject::SceneObject(){

	//リストに追加

	worldTransform_.Initialize();
}

void SceneObject::ShowGui(){
	ImGui::Dummy(ImVec2(0.0f, 5.0f));
	ImGui::Separator();

	if (ImGui::Button("SaveConfig")) {
		SaveConfig(configPath_);
	}
	ImGui::SameLine();
	if (ImGui::Button("LoadConfig")) {
		LoadConfig(configPath_);
	}

	// ImGui で編集

	//===========================
	// 1. カメラ取得
	//===========================
	CameraManager* camMgr = CameraManager::GetInstance();
	if (!camMgr){
		ImGui::Text("CameraManager not found!");
		return;
	}

	BaseCamera* camera = camMgr->GetActiveCamera();
	if (!camera){
		ImGui::Text("No Active Camera found!");
		return;
	}

	//===========================
	// 2. カメラ行列 (View, Proj) を列優先に転置して float[16] へ
	//===========================
	// カメラのView行列とProjection行列を row-major(左手系DX) → column-major(右手系OpenGL) に転置
	Matrix4x4 rowViewMat = camera->GetViewMatrix();
	Matrix4x4 rowProjMat = camera->GetProjectionMatrix();
	/*Matrix4x4 colViewMat = Matrix4x4::Transpose(rowViewMat);
	Matrix4x4 colProjMat = Matrix4x4::Transpose(rowProjMat);*/

	float view[16];
	float proj[16];
	memcpy(view, &rowViewMat, sizeof(view));
	memcpy(proj, &rowProjMat, sizeof(proj));

	//===========================
	// 3. モデルのローカル行列を column-major にして渡す
	//===========================
	// transform.scale, rotate, translate から行列を作る (row-major)
	Matrix4x4 localRM = MakeAffineMatrix(worldTransform_.scale,
										 worldTransform_.eulerRotation,
										 worldTransform_.translation);

	// 親（parent_）がある場合は、さらに掛け合わせるならここで合成
	// (親の行列も row-major で取得 → multiply)
	if (worldTransform_.parent){
		Matrix4x4 parentRM = MakeAffineMatrix(worldTransform_.parent->scale,
											  worldTransform_.parent->eulerRotation,
											  worldTransform_.parent->translation);
		localRM = Matrix4x4::Multiply(parentRM, localRM);  // 親の行列を左側に掛ける
	}

	// row-major → column-major
	//Matrix4x4 localCM = Matrix4x4::Transpose(localRM);

	float model[16];
	memcpy(model, &localRM, sizeof(model));

	//===========================
	// 4. ギズモ表示
	//===========================

	// 操作モード (Translate / Rotate / Scale)
	static ImGuizmo::OPERATION operation = ImGuizmo::TRANSLATE;
	// 空間モード (LOCAL / WORLD)
	static ImGuizmo::MODE gizmoMode = ImGuizmo::LOCAL;

	ImGuizmo::Manipulate(
		view,           // カメラview (col-major)
		proj,           // カメラproj (col-major)
		operation,      // 操作モード
		gizmoMode,      // ローカル or ワールド
		model           // モデル行列 (in/out col-major)
	);

	//Matrix4x4 identityMatrix = Matrix4x4::MakeIdentity();
	//float identityMat[16];
	//memcpy(identityMat, &identityMatrix, sizeof(identityMat));
	//ImGuizmo::DrawGrid(view, proj, identityMat, 100.0f);

	//===========================
	// 5. ギズモ操作の結果をモデルに反映
	//===========================	

	if (ImGuizmo::IsUsing()){
		// `model` はギズモ操作で更新済み → 再び row-major へ戻す	
		Matrix4x4 updatedCM;
		memcpy(&updatedCM, model, sizeof(updatedCM));
		//Matrix4x4 updatedRM = Matrix4x4::Transpose(updatedCM);

		// 親がある場合、「ワールド行列」になっているので
		// 親の逆行列を掛けてローカル行列を取り出す場合がある。
		if (worldTransform_.parent){
			Matrix4x4 parentRM = MakeAffineMatrix(worldTransform_.parent->scale,
												  worldTransform_.parent->eulerRotation,
												  worldTransform_.parent->translation);
			Matrix4x4 parentInv = Matrix4x4::Inverse(parentRM);
			updatedCM = Matrix4x4::Multiply(updatedCM, parentInv);
		}

		// row-major行列 → (S,R,T) に分解
		Vector3 newScale, newRotate, newTrans;
		DecomposeMatrix(updatedCM, newScale, newRotate, newTrans);

		// 変更を自身の transform に反映
		worldTransform_.scale = newScale;
		worldTransform_.eulerRotation = newRotate;  // オイラー角
		worldTransform_.translation = newTrans;
	}

	// ギズモモード切替UI
	if (ImGui::RadioButton("Translate", operation == ImGuizmo::TRANSLATE)){
		operation = ImGuizmo::TRANSLATE;
	}
	ImGui::SameLine();
	if (ImGui::RadioButton("Rotate", operation == ImGuizmo::ROTATE)){
		operation = ImGuizmo::ROTATE;
	}
	ImGui::SameLine();
	if (ImGui::RadioButton("Scale", operation == ImGuizmo::SCALE)){
		operation = ImGuizmo::SCALE;
	}

	worldTransform_.ShowImGui("world");

}

void SceneObject::EnableGuiList() {
}

void SceneObject::SetName(const std::string& name, ObjectType type){
	name_ = name;
	objectType_ = type;
}
