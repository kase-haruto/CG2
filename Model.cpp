#include "Model.h"
#include"DirectXCommon.h"
#include"MyFunc.h"
#include"VertexData.h"

#ifdef _DEBUG
	#include"imgui.h"
#endif // _DEBUG

Model::Model(){}

Model::~Model(){}

void Model::Initialize(DirectXCommon* dxCommon){
	device_ = dxCommon->GetDevice();
	commandList_ = dxCommon->GetCommandList();
	rootSignature_ = dxCommon->GetRootSignature();
	
	//モデルの読み込み
	modelData = LoadObjFile("Resources", "plane.obj");
	CreateBuffer();
	Map();
}

void Model::Update(){
	
}

void Model::Draw(){

}

void Model::CreateBuffer(){
	vertexResource_ = CreateBufferResource(device_.Get(), sizeof(VertexData) * modelData.vertices.size());
	vertexBufferView.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	vertexBufferView.SizeInBytes = UINT(sizeof(VertexData) * modelData.vertices.size());
	vertexBufferView.StrideInBytes = sizeof(VertexData);
}

void Model::Map(){
	//リソースにデータを書き込む
	VertexData* vertexData = nullptr;
	vertexResource_->Map(0, nullptr, reinterpret_cast< void** >(&vertexData));
	std::memcpy(vertexData, modelData.vertices.data(), sizeof(VertexData) * modelData.vertices.size());//頂点データをリソースにコピー
}