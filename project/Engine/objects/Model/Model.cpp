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
	Initialize();
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

	worldTransform_.Initialize();

	materialParameter_.shininess = 20.0f;

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

void Model::Draw(){
	if (!modelData_){
		return;
	}

	GraphicsGroup::GetInstance()->SetCommand(commandList_, Object3D, blendMode_);
	// 頂点バッファ/インデックスバッファをセット
	modelData_->vertexBuffer.SetCommand(commandList_);
	modelData_->indexBuffer.SetCommand(commandList_);
	BaseModel::Draw();
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
	// materialData_ に初期値をセットする
	materialData_.color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	materialData_.shininess = 20.0f;
	materialData_.enableLighting = HalfLambert;
	materialData_.uvTransform = Matrix4x4::MakeIdentity();

	materialBuffer_.Initialize(device_.Get());
}


void Model::MaterialBufferMap(){
	// materialData_ の内容で GPU に転送
	// マテリアルのデータを転送
	materialBuffer_.TransferData(materialData_);
}

