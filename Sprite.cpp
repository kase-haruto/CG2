#include "Sprite.h"
#include"DirectXCommon.h"
#include"MyFunc.h"
#include"VertexData.h"
#include"imgui.h"
#include"TextureManager.h"
#include<stdint.h>

Sprite::Sprite(DirectXCommon* dx) :dxCommon_(dx){

}

Sprite::~Sprite(){

}

void Sprite::Initialize(){
	commandList_ = dxCommon_->GetCommandList();
	device_ = dxCommon_->GetDevice();

	CreateBuffer();
	Map();
}

void Sprite::Update(){
	ImGui::Begin("sprite");
	ImGui::SliderFloat3("translate", &transform_.translate.x, 0.0f, 1280.0f);
	ImGui::End();

	Matrix4x4 matWorld = MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate);
	Matrix4x4 matView = Matrix4x4::MakeIdentity();
	Matrix4x4 matProjection = MakeOrthographicMatrix(0.0f, 0.0f, 1280.0f, 720.0f, 0.0f, 100.0f);
	Matrix4x4 wvpMatrix = Matrix4x4::Multiply(matWorld,Matrix4x4::Multiply(matView,matProjection));
	*transformData = wvpMatrix;
}

void Sprite::Draw(){
	commandList_->SetGraphicsRootDescriptorTable(4, TextureManager::GetInstance()->GetTextureSrvHandleGPU());
	commandList_->IASetVertexBuffers(0, 1, &vertexBufferViewSprite);
	commandList_->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
	commandList_->IASetIndexBuffer(&indexBufferView);//ibvを設定
	commandList_->SetGraphicsRootConstantBufferView(1, transformResource_->GetGPUVirtualAddress());
	commandList_->DrawIndexedInstanced(6, 1, 0, 0, 0);
}

void Sprite::CreateBuffer(){
	//頂点用リソース
	vertexResource_ = CreateBufferResource(device_.Get(), sizeof(VertexData) * 6);
	//頂点バッファビューを作成	
	//リソースの先頭アドレスから使用する
	vertexBufferViewSprite.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	//使用するリソースsizeは頂点6つ分size
	vertexBufferViewSprite.SizeInBytes = sizeof(VertexData) * 6;
	//1頂点当たりのサイズ
	vertexBufferViewSprite.StrideInBytes = sizeof(VertexData);
	
	//transformtionMatrix用のリソース
	transformResource_ = CreateBufferResource(device_.Get(), sizeof(Matrix4x4));

	//indexResource
	indexResource_ = CreateBufferResource(device_.Get(), sizeof(uint32_t) * 6);
	
	//リソース先頭のアドレスから使う
	indexBufferView.BufferLocation = indexResource_->GetGPUVirtualAddress();
	//使用するリソースのサイズはインデックス6つ分のサイズ
	indexBufferView.SizeInBytes = sizeof(uint32_t) * 6;
	//インデックスはuint32_tとする
	indexBufferView.Format = DXGI_FORMAT_R32_UINT;
}

void Sprite::Map(){
	IndexResourceMap();
	VertexResourceMap();
	TransformResourceMap();
	MaterialResourceMap();
}

void Sprite::IndexResourceMap(){
	//indexResourceにデータを書き込む
	uint32_t* indexData = nullptr;
	indexResource_->Map(0, nullptr, reinterpret_cast< void** >(&indexData));
	indexData[0] = 0; indexData[1] = 1; indexData[2] = 2;
	indexData[3] = 1; indexData[4] = 3; indexData[5] = 2;
}

void Sprite::VertexResourceMap(){
	VertexData* vertexData = nullptr;
	vertexResource_->Map(0, nullptr, reinterpret_cast< void** >(&vertexData));

	//1枚目の三角形
	vertexData[0].position = {0.0f,360.0f,0.0f,1.0f};//左下
	vertexData[0].texcoord = {0.0f,1.0f};
	vertexData[0].normal = {0.0f,0.0f,-1.0f};
	vertexData[1].position = {0.0f,0.0f,0.0f,1.0f};//左下
	vertexData[1].texcoord = {0.0f,0.0f};
	vertexData[1].normal = {0.0f,0.0f,-1.0f};
	vertexData[2].position = {640.0f,360.0f,0.0f,1.0f};//左下
	vertexData[2].texcoord = {1.0f,1.0f};
	vertexData[3].normal = {0.0f,0.0f,-1.0f};

	//2枚目の三角形
	vertexData[3].position = {0.0f,0.0f,0.0f,1.0f};//左下
	vertexData[3].texcoord = {0.0f,0.0f};
	vertexData[3].normal = {0.0f,0.0f,-1.0f};
	vertexData[4].position = {640.0f,0.0f,0.0f,1.0f};//左下
	vertexData[4].texcoord = {1.0f,0.0f};
	vertexData[4].normal = {0.0f,0.0f,-1.0f};
	vertexData[5].position = {640.0f,360.0f,0.0f,1.0f};//左下
	vertexData[5].texcoord = {1.0f,1.0f};
	vertexData[5].normal = {0.0f,0.0f,-1.0f};

}

void Sprite::TransformResourceMap(){
	transformResource_->Map(0, nullptr, reinterpret_cast< void** >(&transformData));
	//単位行列を書き込んでおく
	*transformData = Matrix4x4::MakeIdentity();
}

void Sprite::MaterialResourceMap(){
	materialResource_->Map(0, nullptr, reinterpret_cast< void** >(&materialData_));
	materialData_->color = {1.0f,1.0f,1.0f,1.0f};
	materialData_->enableLighting = false;
}



