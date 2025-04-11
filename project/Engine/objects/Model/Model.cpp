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

void Model::Draw(){
	if (!modelData_){
		return;
	}

	GraphicsGroup::GetInstance()->SetCommand(commandList_, Object3D, blendMode_);

	BaseModel::Draw();
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

//==============================================================================
// バッファ生成/マッピング
//==============================================================================
void Model::CreateMaterialBuffer(){
	// materialData_ が未確保なら確保
	if (!materialData_){
		materialData_ = new Material();
	}
	// materialData_ に初期値をセットする
	materialData_->color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	materialData_->shininess = 20.0f;
	materialData_->enableLighting = HalfLambert;
	materialData_->uvTransform = Matrix4x4::MakeIdentity();

	// materialData_ の内容で GPU に転送
	materialBuffer_.Initialize(device_.Get(), 1, materialData_);
}

void Model::CreateMatrixBuffer(){
	matrixData_ = new TransformationMatrix();
	matrixData_->WVP = Matrix4x4::MakeIdentity();
	matrixData_->world = Matrix4x4::MakeIdentity();

	wvpBuffer_.Initialize(
		device_.Get(),
		1,
		matrixData_
	);
}

void Model::MaterialBufferMap(){
	// マテリアルのデータを転送
	materialBuffer_.TransferData(materialData_, 1);
}

void Model::MatrixBufferMap(){
	// ワールド行列と WVP 行列のデータを転送
	wvpBuffer_.TransferData(matrixData_, 1);
}