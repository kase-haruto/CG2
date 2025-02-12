#include "BaseModel.h"

#include "lib/myFunc/MyFunc.h"

#include "../ModelManager.h"
#include "../TextureManager.h"

#include "Engine/graphics/camera/CameraManager.h"

#include "externals/imgui/imgui.h"
#include "externals/imgui/ImGuizmo.h"

const std::string BaseModel::directoryPath_ = "Resource/models";

Matrix4x4 BaseModel::GetWorldRotationMatrix(){
	// 現在のオブジェクトのローカル回転行列を取得
	Matrix4x4 localRot = EulerToMatrix(transform.rotate);

	// 親が存在する場合、親のワールド回転行列と合成する
	if (parent_ != nullptr){
		Matrix4x4 parentWorldRot = EulerToMatrix(parent_->translate);
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

            // 頂点バッファビューをセットアップ
            vertexBufferView_.BufferLocation = modelData_->vertexBufferResource->GetGPUVirtualAddress();
            vertexBufferView_.SizeInBytes = UINT(sizeof(VertexData) * modelData_->vertices.size());
            vertexBufferView_.StrideInBytes = sizeof(VertexData);

            // インデックスバッファビューをセットアップ
            indexBufferView_.BufferLocation = modelData_->indexBufferResource->GetGPUVirtualAddress();
            indexBufferView_.SizeInBytes = UINT(sizeof(uint32_t) * modelData_->indices.size());
            indexBufferView_.Format = DXGI_FORMAT_R32_UINT;

            // テクスチャ設定
            handle_ = TextureManager::GetInstance()->LoadTexture(modelData_->material.textureFilePath);

			// マテリアル・行列バッファ生成
            UpdateMatrix();
        }
        // loaded が nullptr の場合、まだ読み込み中 → 次フレーム以降に再試行
    }
}

void BaseModel::ShowImGuiInterface(){
    //===========================
  // 1. カメラ取得
  //===========================
  // 例：現在アクティブなカメラを取得
  //     カメラの種類は CameraManager の内部状態で切り替わっていると想定
    CameraManager* camMgr = CameraManager::GetInstance();
    if (!camMgr){
        ImGui::Text("CameraManager not found!");
        return;
    }

    BaseCamera* camera = camMgr->GetBaseCamera();
    if (!camera){
        ImGui::Text("No Active Camera found!");
        return;
    }

    //===========================
    // 2. カメラ行列 (View, Proj) を列優先に転置して float[16] へ
    //===========================
    // row-major (DX) → column-major (imGuizmo)
    const Matrix4x4& rowViewMat = camera->GetViewMatrix();
    const Matrix4x4& rowProjMat = camera->GetViewProjection();

    Matrix4x4 colViewMat = Matrix4x4::Transpose(rowViewMat);
    Matrix4x4 colProjMat = Matrix4x4::Transpose(rowProjMat);

    float view[16];
    float proj[16];
    memcpy(view, &colViewMat, sizeof(view));
    memcpy(proj, &colProjMat, sizeof(proj));

    //===========================
    // 3. モデルのローカル行列を column-major にして渡す
    //===========================
    // transform.scale, rotate, translate から行列を作る (row-major)
    Matrix4x4 localRM = MakeAffineMatrix(transform.scale, transform.rotate, transform.translate);

    // 親（parent_）がある場合は、さらに掛け合わせるならここで合成
    // (親の行列も row-major で取得 → multiply)
    if (parent_){
        Matrix4x4 parentRM = MakeAffineMatrix(parent_->scale, parent_->rotate, parent_->translate);
        localRM = Matrix4x4::Multiply(localRM, parentRM);
    }

    // row-major → column-major
    Matrix4x4 localCM = Matrix4x4::Transpose(localRM);

    float model[16];
    memcpy(model, &localCM, sizeof(model));

    //===========================
    // 4. ギズモ表示
    //===========================
    // ギズモを描画する領域 (ここでは画面全体)
    ImGuizmo::SetRect(
        0.0f,
        0.0f,
        ImGui::GetIO().DisplaySize.x,
        ImGui::GetIO().DisplaySize.y
    );

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

    //===========================
    // 5. ギズモ操作の結果をモデルに反映
    //===========================
    if (ImGuizmo::IsUsing()){
        // `model` はギズモ操作で更新済み → 再び row-major へ戻す
        Matrix4x4 updatedCM;
        memcpy(&updatedCM, model, sizeof(updatedCM));
        Matrix4x4 updatedRM = Matrix4x4::Transpose(updatedCM);

        // 親がある場合、「ワールド行列」になっているので
        // 親の逆行列を掛けてローカル行列を取り出す場合がある。
        if (parent_){
            Matrix4x4 parentRM = MakeAffineMatrix(parent_->scale, parent_->rotate, parent_->translate);
            Matrix4x4 parentInv = Matrix4x4::Inverse(parentRM);
            updatedRM = Matrix4x4::Multiply(updatedRM, parentInv);
        }

        // row-major行列 → (S,R,T) に分解
        Vector3 newScale, newRotate, newTrans;
        DecomposeMatrix(updatedRM, newScale, newRotate, newTrans);

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

    // ギズモモード切替UI (例)
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
}
