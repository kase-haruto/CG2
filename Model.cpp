#include "Model.h"
#include"MyFunc.h"
#include"VertexData.h"
#include"TextureManager.h"

#include"GraphicsGroup.h"

#ifdef _DEBUG
	#include"imgui.h"
#endif // _DEBUG

Model::Model(){}

Model::~Model(){ 
	pipelineState_.Reset();
	device_.Reset();
	commandList_.Reset();
	pipelineState_.Reset();
	rootSignature_.Reset();
}

void Model::Initialize(bool isUseTexture){
	device_ = GraphicsGroup::GetInstance()->GetDevice();
	commandList_ = GraphicsGroup::GetInstance()->GetCommandList();

	//パイプラインを設定
	PipelineType pipelineType = isUseTexture ? Object3D : UntexturedModel;
	rootSignature_ = GraphicsGroup::GetInstance()->GetRootSignature(pipelineType);
	pipelineState_ = GraphicsGroup::GetInstance()->GetPipelineState(pipelineType);
	
	RGBa = {1.0f,1.0f,1.0f,1.0f};
	transform = {{1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f}};

	///=================================================
	///		リソースの生成と書き込み
	///=================================================
	CreateBuffer();
	Map();
}

void Model::Create(const std::string& directoryPath, const std::string& filename, bool isUseTexture){
	//モデルの読み込み
	modelData = LoadObjFile(directoryPath.c_str(), filename.c_str());
	Initialize();
}

void Model::Update(){
#ifdef _DEBUG
	ImGui::Begin("model");
	ImGui::DragFloat3("translation", &transform.translate.x, 0.01f);
	ImGui::DragFloat3("rotation", &transform.rotate.x, 0.01f);
	ImGui::ColorEdit4("color", &RGBa.x);
	bool enableLighting = (materialData->enableLighting != 0);
	ImGui::Checkbox("enableLighting", &enableLighting);
	materialData->enableLighting = enableLighting ? 1 : 0;
	ImGui::End();
#endif // _DEBUG

	materialData->color = Vector4(RGBa.x, RGBa.y, RGBa.z, RGBa.w);

	Matrix4x4 worldMatrix = MakeAffineMatrix(transform.scale,
											 transform.rotate,
											 transform.translate
	);

	Matrix4x4 worldViewProjectionMatrix = Matrix4x4::Multiply(worldMatrix, viewProjection_->GetViewProjection());
	matrixData->world = worldMatrix;
	matrixData->WVP = worldViewProjectionMatrix;
}

void Model::Draw(){
	commandList_->SetGraphicsRootSignature(rootSignature_.Get());
	commandList_->SetPipelineState(pipelineState_.Get());
	commandList_->IASetVertexBuffers(0, 1, &vertexBufferView);
	//形状を設定。psoに設定しているものとはまた別。同じものを設定すると考える
	commandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//マテリアルCBufferの場所を設定
	commandList_->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
	//wvp用のCBufferの場所を設定
	commandList_->SetGraphicsRootConstantBufferView(1, wvpResource_->GetGPUVirtualAddress());
	//srvのdescriptorTableの先頭を設定。3はrootParamenter[3]
	commandList_->SetGraphicsRootDescriptorTable(3,TextureManager::GetInstance()->GetTextureSrvHandle2());
	//モデル
	commandList_->DrawInstanced(UINT(modelData.vertices.size()), 1, 0, 0);
}


void Model::CreateBuffer(){
	CreateVertexBuffer();
	CreateMaterialBuffer();
	CreateMatrixBuffer();
}

void Model::CreateVertexBuffer(){
	vertexResource_ = CreateBufferResource(device_.Get(), sizeof(VertexData) * modelData.vertices.size());
	vertexBufferView.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	vertexBufferView.SizeInBytes = UINT(sizeof(VertexData) * modelData.vertices.size());
	vertexBufferView.StrideInBytes = sizeof(VertexData);
}

void Model::CreateMaterialBuffer(){
	materialResource_ = CreateBufferResource(device_.Get(), sizeof(Material));
}

void Model::CreateMatrixBuffer(){
	wvpResource_ = CreateBufferResource(device_.Get(), sizeof(TransformationMatrix));
}


void Model::Map(){
	//リソースにデータを書き込む
	VertexBufferMap();
	MaterialBufferMap();
	MatrixBufferMap();
}

void Model::VertexBufferMap(){
	VertexData* vertexData = nullptr;
	//書き込むためのアドレスを取得
	vertexResource_->Map(0, nullptr,
						reinterpret_cast< void** >(&vertexData));
	//モデル用
	std::memcpy(vertexData, modelData.vertices.data(), sizeof(VertexData) * modelData.vertices.size());
}

void Model::MaterialBufferMap(){
	materialResource_->Map(0, nullptr, reinterpret_cast< void** >(&materialData));
	//今回はあかをかきこむ
	materialData->color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	materialData->enableLighting = true;
	materialData->uvTransform = Matrix4x4::MakeIdentity();
}

void Model::MatrixBufferMap(){
	wvpResource_->Map(0, nullptr, reinterpret_cast< void** >(&matrixData));
	//単位行列を書き込んでおく
	matrixData->WVP = Matrix4x4::MakeIdentity();
}

void Model::SetViewProjection(ViewProjection* viewPro){
	viewProjection_ = viewPro;
}
