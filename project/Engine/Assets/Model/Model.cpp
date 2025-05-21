#include "Model.h"
/* ========================================================================
/* include space
/* ===================================================================== */
// engine
#include <Engine/Foundation/Clock/ClockManager.h>
#include <Engine/Application/System/System.h>
#include "engine/graphics/Material.h"
#include <Engine/Objects/Transform/Transform.h>
#include <Engine/Objects/Transform/TransformationMatrix.h>
#include <Engine/Foundation/Math/Vector4.h>
#include <Engine/Assets/Model/ModelData.h>
#include <Engine/Assets/Model/ModelManager.h>
#include <Engine/Assets/Texture/TextureManager.h>
#include <Engine/Graphics/Context/GraphicsGroup.h>
#include <Engine/Renderer/Mesh/VertexData.h>
#include <Engine/Lighting/LightData.h>

#ifdef _DEBUG
#include "externals/imgui/imgui.h"
#endif

#include <Engine/Foundation/Utility/Func/MyFunc.h>

Model::Model(const std::string& fileName){
	fileName_ = fileName;
	Initialize();
}

Model::~Model(){
	// 必要があれば後処理
}

void Model::Initialize(){
	// マテリアル・行列バッファ生成
	CreateMaterialBuffer();
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

void Model::Draw(const WorldTransform& transform){
	if (!modelData_){
		return;
	}
	ID3D12GraphicsCommandList* cmdList = GraphicsGroup::GetInstance()->GetCommandList().Get();
	GraphicsGroup::GetInstance()->SetCommand(cmdList, Object3D, blendMode_);
	// 頂点バッファ/インデックスバッファをセット
	modelData_->vertexBuffer.SetCommand(cmdList);
	modelData_->indexBuffer.SetCommand(cmdList);
	BaseModel::Draw(transform);
}

void Model::Map(){
	// マテリアルと行列のマッピング
	MaterialBufferMap();
}

void Model::ShowImGuiInterface(){
#ifdef _DEBUG
	BaseModel::ShowImGuiInterface();
#endif
}

//==============================================================================
// バッファ生成/マッピング
//==============================================================================
void Model::CreateMaterialBuffer(){
	ID3D12Device* device = GraphicsGroup::GetInstance()->GetDevice().Get();
	// materialData_ に初期値をセットする
	materialData_.color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	materialData_.shininess = 20.0f;
	materialData_.enableLighting = HalfLambert;
	materialData_.uvTransform = Matrix4x4::MakeIdentity();

	materialBuffer_.Initialize(device);
}

void Model::MaterialBufferMap(){
	// materialData_ の内容で GPU に転送
	// マテリアルのデータを転送
	materialBuffer_.TransferData(materialData_);
}

