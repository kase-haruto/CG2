#include "BaseModel.h"

#include "lib/myFunc/MyFunc.h"

#include "../ModelManager.h"
#include "../TextureManager.h"

#include "Engine/graphics/camera/CameraManager.h"

#include "externals/imgui/imgui.h"
#define IMGUIZMO_USE_LH
#define IMGUIZMO_USE_D3D9_CLIP_SPACE
#include "externals/imgui/ImGuizmo.h"

#include "Engine/core/Clock/ClockManager.h"

const std::string BaseModel::directoryPath_ = "Resource/models";

Matrix4x4 BaseModel::GetWorldRotationMatrix(){
	// 現在のオブジェクトのローカル回転行列を取得
	Matrix4x4 localRot = EulerToMatrix(transform.rotate);

	// 親が存在する場合、親のワールド回転行列と合成する
	if (parent_ != nullptr){
		Matrix4x4 parentWorldRot = EulerToMatrix(parent_->rotate);
		return Matrix4x4::Multiply(parentWorldRot, localRot);
	} else{
		return localRot;
	}
}

void BaseModel::Update(){
	// --- まだ modelData_ を取得していないなら、取得を試みる ---
	if (!modelData_){
		auto loaded = ModelManager::GetInstance()->GetModelData(fileName_);
		if (loaded){
			// ここで初めて GPUリソースが完成した ModelData を受け取れた！
			modelData_ = loaded;

			modelData_->vertexBuffer.Initialize(device_,UINT(modelData_->vertices.size()), modelData_->vertices.data());
			modelData_->indexBuffer.Initialize(device_, UINT(modelData_->indices.size()),modelData_->indices.data());

			// テクスチャ設定
			if (!handle_){
				handle_ = TextureManager::GetInstance()->LoadTexture(modelData_->material.textureFilePath);
			}

			UpdateMatrix();
		}
		// loaded が nullptr の場合、まだ読み込み中 → 次フレーム以降に再試行
	} else{
		// テクスチャの更新
		UpdateTexture();

		// UV transform を行列化 (例: スケール→Z回転→平行移動)
		Matrix4x4 uvTransformMatrix = MakeScaleMatrix(uvTransform.scale);
		uvTransformMatrix = Matrix4x4::Multiply(uvTransformMatrix, MakeRotateZMatrix(uvTransform.rotate.z));
		uvTransformMatrix = Matrix4x4::Multiply(uvTransformMatrix, MakeTranslateMatrix(uvTransform.translate));
		materialData_->uvTransform = uvTransformMatrix;

		// カメラ行列との掛け合わせ
		UpdateMatrix();
		Map();
	}
}

void BaseModel::UpdateMatrix(){
	// ワールド行列の更新
	worldMatrix = MakeAffineMatrix(transform.scale, transform.rotate, transform.translate);
	// 親の行列がある場合は親の行列を掛け合わせる
	if (parent_ != nullptr){
		Matrix4x4 parentWorldMat = MakeAffineMatrix(parent_->scale, parent_->rotate, parent_->translate);
		worldMatrix = Matrix4x4::Multiply(worldMatrix, parentWorldMat);
	}

	// もし外部から行列のみを更新したい場合などに呼ばれる
	Matrix4x4 worldViewProjectionMatrix = Matrix4x4::Multiply(worldMatrix, CameraManager::GetViewProjectionMatrix());
	matrixData_->world = worldMatrix;
	matrixData_->WVP = worldViewProjectionMatrix;
}

void BaseModel::UpdateTexture(){
	if (textureHandles_.size() <= 1) return; // アニメーション不要
	elapsedTime_ += ClockManager::GetInstance()->GetDeltaTime();
	if (elapsedTime_ >= animationSpeed_){
		elapsedTime_ -= animationSpeed_;
		currentFrameIndex_ = (currentFrameIndex_ + 1) % textureHandles_.size();
		handle_ = textureHandles_[currentFrameIndex_]; // テクスチャを切り替え
	}
}

void BaseModel::ShowImGuiInterface(){
	// ImGui で編集

	//===========================
	// 1. カメラ取得
	//===========================
	// 例：現在アクティブなカメラを取得
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
	Matrix4x4 localRM = MakeAffineMatrix(transform.scale, transform.rotate, transform.translate);

	// 親（parent_）がある場合は、さらに掛け合わせるならここで合成
	// (親の行列も row-major で取得 → multiply)
	if (parent_){
		Matrix4x4 parentRM = MakeAffineMatrix(parent_->scale, parent_->rotate, parent_->translate);
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
		if (parent_){
			Matrix4x4 parentRM = MakeAffineMatrix(parent_->scale, parent_->rotate, parent_->translate);
			Matrix4x4 parentInv = Matrix4x4::Inverse(parentRM);
			updatedCM = Matrix4x4::Multiply(updatedCM, parentInv);
		}

		// row-major行列 → (S,R,T) に分解
		Vector3 newScale, newRotate, newTrans;
		DecomposeMatrix(updatedCM, newScale, newRotate, newTrans);

		// 変更を自身の transform に反映
		transform.scale = newScale;
		transform.rotate = newRotate;  // オイラー角
		transform.translate = newTrans;
	}

	//===========================
	// 6. 追加の GUI 表示
	//===========================
	ImGui::Separator();
	ImGui::Text("Model: %s", fileName_.c_str());

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


	if (ImGui::BeginTabBar("##InspectorTabs")){

		if (ImGui::BeginTabItem("Transform")){
			transform.ShowImGui();
			uvTransform.ShowImGui("UV Transform");
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Material")){
			materialData_->ShowImGui();
			static std::string selectedTextureName = modelData_->material.textureFilePath;
			//テクスチャの切り替え
			auto& textures = TextureManager::GetInstance()->GetLoadedTextures();
			if (ImGui::BeginCombo("Texture", selectedTextureName.c_str())){
				for (const auto& texture : textures){
					bool is_selected = (selectedTextureName == texture.first);
					if (ImGui::Selectable(texture.first.c_str(), is_selected)){
						selectedTextureName = texture.first; // 選択したテクスチャ名を更新
						handle_ = TextureManager::GetInstance()->LoadTexture(texture.first); // テクスチャを変更
					}
					if (is_selected){
						ImGui::SetItemDefaultFocus();
					}
				}
				ImGui::EndCombo();
			}
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Draw")){
			static const char* blendModeNames[] = {
				"NONE",
				"ALPHA",
				"ADD",
				"SUB",
				"MUL",
				"NORMAL",
				"SCREEN"
			};

			int currentBlendMode = static_cast< int >(blendMode_);
			if (ImGui::Combo("Blend Mode", &currentBlendMode, blendModeNames, IM_ARRAYSIZE(blendModeNames))){
				blendMode_ = static_cast< BlendMode >(currentBlendMode);
			}

			ImGui::EndTabItem();
		}


		ImGui::EndTabBar();
	}


}

void BaseModel::Draw(){
	if (!modelData_){
		return;
	}

	GraphicsGroup::GetInstance()->SetCommand(commandList_, Object3D, blendMode_);

	commandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// 頂点バッファ/インデックスバッファをセット
	modelData_->vertexBuffer.SetCommand(commandList_);
	modelData_->indexBuffer.SetCommand(commandList_);


	// マテリアル & 行列バッファをセット
	materialBuffer_.SetCommand(commandList_, 0);
	wvpBuffer_.SetCommand(commandList_, 1);

	commandList_->SetGraphicsRootDescriptorTable(3, handle_.value());

	// 描画
	commandList_->DrawIndexedInstanced(UINT(modelData_->indices.size()), 1, 0, 0, 0);
}