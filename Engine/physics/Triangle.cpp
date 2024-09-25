#include "Triangle.h"
#include"DirectXCommon.h"
#include"MyFunc.h"
#include"TextureManager.h"
#include"imgui.h"
#include"DirectionalLight.h"

#include"GraphicsGroup.h"

#include<numbers>

Triangle::Triangle(){}

void Triangle::Initialize(ViewProjection* viewProjection){
	auto graphics = GraphicsGroup::GetInstance();
	device_ = graphics->GetDevice();
	commandList_ = graphics->GetCommandList();
	rootSignature_ = graphics->GetRootSignature(Object3D);
	pipelineState_ = graphics->GetPipelineState(Object3D);

	viewProjection_ = viewProjection;

	RGBa = {1.0f,1.0f,1.0f,1.0f};
	transform = {{1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f}};

	///=================================================
	///		リソースの生成と書き込み
	///=================================================
	CreateBuffer();
	Map();
}

void Triangle::Update(){
	materialData->color = Vector4(RGBa.x, RGBa.y, RGBa.z, RGBa.w);
	MatrixInitialize();
}

void Triangle::UpdateImGui(std::string lavel){
#ifdef _DEBUG
	ImGui::Begin(lavel.c_str());
	ImGui::DragFloat3("translation", &transform.translate.x, 0.01f);
	ImGui::DragFloat3("rotation", &transform.rotate.x, 0.01f);
	ImGui::ColorEdit4("color", &RGBa.x);
	bool enableLighting = (materialData->enableLighting != 0);
	ImGui::Checkbox("enableLighting", &enableLighting);
	materialData->enableLighting = enableLighting ? 1 : 0;
	ImGui::End();
#endif // _DEBUG
}

void Triangle::Draw(){
	//デフォルトのテクスチャ -> uvChecker
	D3D12_GPU_DESCRIPTOR_HANDLE handle = TextureManager::GetInstance()->LoadTexture("./Resources/uvChecker.png");
	commandList_->SetPipelineState(pipelineState_.Get());
	commandList_->SetGraphicsRootSignature(rootSignature_.Get());
	commandList_->IASetVertexBuffers(0, 1, &vertexBufferView);
	//形状を設定。psoに設定しているものとはまた別。同じものを設定すると考える
	commandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//マテリアルCBufferの場所を設定
	commandList_->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
	//wvp用のCBufferの場所を設定
	commandList_->SetGraphicsRootConstantBufferView(1, wvpResource_->GetGPUVirtualAddress());
	//srvのdescriptorTableの先頭を設定。3はrootParamenter[3]
	commandList_->SetGraphicsRootDescriptorTable(3, handle);
	//描画　3頂点で1つのインスタンス
	commandList_->DrawInstanced(3, 1, 0, 0);
}

void Triangle::CreateBuffer(){
	CreateVertexBuffer();
	CreateMaterialBuffer();
	CreateMatrixBuffer();
}

void Triangle::CreateVertexBuffer(){
	vertexResource_ = CreateBufferResource(device_.Get(), sizeof(VertexData) * 3);
	vertexBufferView.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	vertexBufferView.SizeInBytes = UINT(sizeof(VertexData) * 3);
	vertexBufferView.StrideInBytes = sizeof(VertexData);
}

void Triangle::CreateMaterialBuffer(){
	materialResource_ = CreateBufferResource(device_.Get(), sizeof(Material));
}

void Triangle::CreateMatrixBuffer(){
	wvpResource_ = CreateBufferResource(device_.Get(), sizeof(TransformationMatrix));
}


void Triangle::Map(){
	//リソースにデータを書き込む
	VertexBufferMap();
	MaterialBufferMap();
	MatrixBufferMap();
}

void Triangle::VertexBufferMap(){
	VertexData* vertexData = nullptr;
	//書き込むためのアドレスを取得
	vertexResource_->Map(0, nullptr,
						 reinterpret_cast< void** >(&vertexData));
	//左下
	vertexData[0].position = {-0.5f,-0.5f,0.0f,1.0f};
	vertexData[0].texcoord = {0.0f,1.0f};
	vertexData[0].normal = {0.0f,0.0f,-1.0f};
	//上
	vertexData[1].position = {0.0f,0.5f,0.0f,1.0f};
	vertexData[1].texcoord = {0.5f,0.0f};
	vertexData[1].normal = {0.0f,0.0f,-1.0f};
	//右下
	vertexData[2].position = {0.5f,-0.5f,0.0f,1.0f};
	vertexData[2].texcoord = {1.0f,1.0f};
	vertexData[2].normal = {0.0f,0.0f,-1.0f};
}

void Triangle::MaterialBufferMap(){
	materialResource_->Map(0, nullptr, reinterpret_cast< void** >(&materialData));
	materialData->color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	materialData->enableLighting = HalfLambert;
	materialData->uvTransform = Matrix4x4::MakeIdentity();
}

void Triangle::MatrixBufferMap(){
	wvpResource_->Map(0, nullptr, reinterpret_cast< void** >(&matrixData));
	//単位行列を書き込んでおく
	MatrixInitialize();
}

void Triangle::MatrixInitialize(){
	Matrix4x4 worldMatrix = MakeAffineMatrix(transform.scale,
											 transform.rotate,
											 transform.translate
	);

	Matrix4x4 worldViewProjectionMatrix = Matrix4x4::Multiply(worldMatrix, viewProjection_->GetViewProjection());
	matrixData->world = worldMatrix;
	matrixData->WVP = worldViewProjectionMatrix;
}