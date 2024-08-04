#include "Particle.h"
#include"DirectXCommon.h"
#include"MyFunc.h"
#include"VertexData.h"
#include"TextureManager.h"
#include"GraphicsGroup.h"

#ifdef _DEBUG
#include"imgui.h"
#endif // _DEBUG

Particle::Particle(){}

Particle::~Particle(){}

void Particle::Initialize(ViewProjection* viewProjection){
	commandList_ = GraphicsGroup::GetInstance()->GetCommandList();
	/*rootSignature_ = dxCommon->GetRootSignature();
	pipelineState_ = dxCommon->GetPipelineState();*/
	viewProjection_ = viewProjection;

	//モデルの読み込み
	modelData = LoadObjFile("Resources", "plane.obj");
	RGBa = {1.0f,1.0f,1.0f,1.0f};
	transform = {{1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f}};

	///=================================================
	///		リソースの生成と書き込み
	///=================================================
	CreateBuffer();
	Map();
}

void Particle::Update(){
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

void Particle::Draw(){
	//デフォルトのテクスチャ -> uvChecker
	D3D12_GPU_DESCRIPTOR_HANDLE handle = TextureManager::GetInstance()->LoadTexture("./Resources/uvChecker.png");

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
	commandList_->SetGraphicsRootDescriptorTable(3, handle);
	//モデル
	commandList_->DrawInstanced(UINT(modelData.vertices.size()), 1, 0, 0);
}


void Particle::CreateBuffer(){
	CreateVertexBuffer();
	CreateMaterialBuffer();
	CreateMatrixBuffer();
}

void Particle::CreateVertexBuffer(){
	vertexResource_ = CreateBufferResource(device_.Get(), sizeof(VertexData) * modelData.vertices.size());
	vertexBufferView.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	vertexBufferView.SizeInBytes = UINT(sizeof(VertexData) * modelData.vertices.size());
	vertexBufferView.StrideInBytes = sizeof(VertexData);
}

void Particle::CreateMaterialBuffer(){
	materialResource_ = CreateBufferResource(device_.Get(), sizeof(Material));
}

void Particle::CreateMatrixBuffer(){
	wvpResource_ = CreateBufferResource(device_.Get(), sizeof(TransformationMatrix));
}


void Particle::Map(){
	//リソースにデータを書き込む
	VertexBufferMap();
	MaterialBufferMap();
	MatrixBufferMap();
}

void Particle::VertexBufferMap(){
	VertexData* vertexData = nullptr;
	//書き込むためのアドレスを取得
	vertexResource_->Map(0, nullptr,
						 reinterpret_cast< void** >(&vertexData));
	//モデル用
	std::memcpy(vertexData, modelData.vertices.data(), sizeof(VertexData) * modelData.vertices.size());
}

void Particle::MaterialBufferMap(){
	materialResource_->Map(0, nullptr, reinterpret_cast< void** >(&materialData));
	//今回はあかをかきこむ
	materialData->color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	materialData->enableLighting = true;
	materialData->uvTransform = Matrix4x4::MakeIdentity();
}

void Particle::MatrixBufferMap(){
	wvpResource_->Map(0, nullptr, reinterpret_cast< void** >(&matrixData));
	//単位行列を書き込んでおく
	matrixData->WVP = Matrix4x4::MakeIdentity();
}
