#include "BaseModel.h"

#include "lib/myFunc/MyFunc.h"

#include "../ModelManager.h"
#include "../TextureManager.h"

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
        }
        // loaded が nullptr の場合、まだ読み込み中 → 次フレーム以降に再試行
    }
}
