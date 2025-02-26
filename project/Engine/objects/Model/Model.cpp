#include "Model.h"

#include "engine/objects/ModelData.h"
#include "engine/graphics/Material.h"
#include "engine/objects/TransformationMatrix.h"
#include "engine/objects/Transform.h"
#include "lib/myMath/Vector4.h"

#include "engine/graphics/GraphicsGroup.h"
#include "engine/objects/TextureManager.h"
#include "engine/objects/ModelManager.h"
#include "engine/graphics/VertexData.h"
#include "Engine/graphics/camera/CameraManager.h"
#include "Engine/core/System.h"
#include "Engine/core/Clock/ClockManager.h"

#include "Engine/physics/light/LightManager.h"

#ifdef _DEBUG
#include "externals/imgui/imgui.h"
#endif

#include "lib/myfunc/MyFunc.h" // 必要に応じて

Model::Model(const std::string& fileName){
	fileName_ = fileName;
	Create(fileName);
}

Model::~Model(){
	// 必要があれば後処理
}

void Model::Initialize(){
	// Device, CommandListの取得
	device_ = GraphicsGroup::GetInstance()->GetDevice();
	commandList_ = GraphicsGroup::GetInstance()->GetCommandList();


	// デフォルト値
	RGBa = {1.0f, 1.0f, 1.0f, 1.0f};
	transform = {{1.0f, 1.0f, 1.0f},
				   {0.0f, 0.0f, 0.0f},
				   {0.0f, 0.0f, 0.0f}};

	materialParameter_.shininess = 20.0f;

	// マテリアル・行列バッファ生成
	CreateMaterialBuffer();
	CreateMatrixBuffer();
	Map();
}

void Model::InitializeTextures(const std::vector<std::string>& textureFilePaths){
	textureHandles_.clear();
	for (const auto& filePath : textureFilePaths){
		textureHandles_.push_back(TextureManager::GetInstance()->LoadTexture(filePath));
	}
	if (!textureHandles_.empty()){
		handle_ = textureHandles_[0]; // 初期テクスチャ
	}
}

void Model::Create(const std::string& filename){
	fileName_ = filename;
	// まずは初期化
	Initialize();
}

void Model::Update(){
	if (modelData_){
		// テクスチャの更新
		UpdateTexture();

		// UV transform を行列化 (例: スケール→Z回転→平行移動)
		Matrix4x4 uvTransformMatrix = MakeScaleMatrix(uvTransform.scale);
		uvTransformMatrix = Matrix4x4::Multiply(uvTransformMatrix, MakeRotateZMatrix(uvTransform.rotate.z));
		uvTransformMatrix = Matrix4x4::Multiply(uvTransformMatrix, MakeTranslateMatrix(uvTransform.translate));
		materialData_->uvTransform = uvTransformMatrix;

		// マテリアルの更新
		materialData_->color = RGBa;

		// ワールド行列の更新
		worldMatrix = MakeAffineMatrix(transform.scale, transform.rotate, transform.translate);

		// 親の行列がある場合は親の行列を掛け合わせる
		if (parent_ != nullptr){
			Matrix4x4 parentWorldMat = MakeAffineMatrix(parent_->scale, parent_->rotate, parent_->translate);
			worldMatrix = Matrix4x4::Multiply(worldMatrix, parentWorldMat);
		}

		// カメラ行列との掛け合わせ
		UpdateMatrix();
	}
	BaseModel::Update();
}

void Model::UpdateTexture(){
	if (textureHandles_.size() <= 1) return; // アニメーション不要
	elapsedTime_ += ClockManager::GetInstance()->GetDeltaTime();
	if (elapsedTime_ >= animationSpeed_){
		elapsedTime_ -= animationSpeed_;
		currentFrameIndex_ = (currentFrameIndex_ + 1) % textureHandles_.size();
		handle_ = textureHandles_[currentFrameIndex_]; // テクスチャを切り替え
	}
}

void Model::UpdateMatrix(){
	// もし外部から行列のみを更新したい場合などに呼ばれる
	Matrix4x4 worldViewProjectionMatrix = Matrix4x4::Multiply(worldMatrix, CameraManager::GetViewProjectionMatrix());
	matrixData_->world = worldMatrix;
	matrixData_->WVP = worldViewProjectionMatrix;
}

void Model::Map(){
	// マテリアルと行列のマッピング
	MaterialBufferMap();
	MatrixBufferMap();
}

void Model::ShowImGuiInterface(){
#ifdef _DEBUG
	BaseModel::ShowImGuiInterface();
#endif
}

void Model::Draw(){
	if (!modelData_){
		return;
	}

	ComPtr<ID3D12PipelineState> pipelineState = GraphicsGroup::GetInstance()->GetPipelineState(Object3D, blendMode_);
	ComPtr<ID3D12RootSignature> rootSignature = GraphicsGroup::GetInstance()->GetRootSignature(Object3D, blendMode_);

	// ルートシグネチャ・パイプラインをセット
	commandList_->SetGraphicsRootSignature(rootSignature.Get());
	commandList_->SetPipelineState(pipelineState.Get());

	// 頂点バッファ/インデックスバッファをセット
	commandList_->IASetVertexBuffers(0, 1, &vertexBufferView_);
	commandList_->IASetIndexBuffer(&indexBufferView_);
	commandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// マテリアル & 行列バッファをセット
	commandList_->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
	commandList_->SetGraphicsRootConstantBufferView(1, wvpResource_->GetGPUVirtualAddress());
	commandList_->SetGraphicsRootDescriptorTable(3, handle_.value());

	// 描画
	commandList_->DrawIndexedInstanced(UINT(modelData_->indices.size()), 1, 0, 0, 0);
}

//==============================================================================
// バッファ生成/マッピング
//==============================================================================
void Model::CreateMaterialBuffer(){
	materialResource_ = CreateBufferResource(device_.Get(), sizeof(Material));
}

void Model::CreateMatrixBuffer(){
	wvpResource_ = CreateBufferResource(device_.Get(), sizeof(TransformationMatrix));
}

void Model::MaterialBufferMap(){
	materialResource_->Map(0, nullptr, reinterpret_cast< void** >(&materialData_));
	materialData_->color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	materialData_->enableLighting = HalfLambert;  // デフォルト
	materialData_->uvTransform = Matrix4x4::MakeIdentity();
	materialData_->shininess = 20.0f;
	materialResource_->Unmap(0, nullptr);
}

void Model::MatrixBufferMap(){
	wvpResource_->Map(0, nullptr, reinterpret_cast< void** >(&matrixData_));
	matrixData_->WVP = Matrix4x4::MakeIdentity();
	matrixData_->world = Matrix4x4::MakeIdentity();
	wvpResource_->Unmap(0, nullptr);
}
