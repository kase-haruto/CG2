﻿#include "Sprite.h"
#include"DirectXCommon.h"
#include"MyFunc.h"
#include"VertexData.h"
#include"imgui.h"
#include"TextureManager.h"
#include"TransformationMatrix.h"

#include<stdint.h>

Sprite::Sprite(DirectXCommon* dx) :dxCommon_(dx){

}

Sprite::~Sprite(){

}

void Sprite::Initialize(){
	commandList_ = dxCommon_->GetCommandList();
	device_ = dxCommon_->GetDevice();

	//===============================
	//	リソースの生成
	CreateBuffer();

	//===============================
	//	マップ
	Map();
}

void Sprite::Update(){
	ImGui::Begin("sprite");
	ImGui::SliderFloat3("translate", &transform_.translate.x, 0.0f, 1280.0f);
	ImGui::DragFloat2("UVTranslate", &uvTransform.translate.x, 0.01f, -10.0f, 10.0f);
	ImGui::DragFloat2("UVScale", &uvTransform.scale.x, 0.01f, -10.0f, 10.0f);
	ImGui::SliderAngle("UVRotate", &uvTransform.rotate.x);
	ImGui::End();

	///===================================================
	///	wvp行列
	///===================================================
	Matrix4x4 matWorld = MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate);
	Matrix4x4 matView = Matrix4x4::MakeIdentity();
	Matrix4x4 matProjection = MakeOrthographicMatrix(0.0f, 0.0f, 1280.0f, 720.0f, 0.0f, 100.0f);
	Matrix4x4 wvpMatrix = Matrix4x4::Multiply(matWorld,Matrix4x4::Multiply(matView,matProjection));
	*transformData = wvpMatrix;


	///===================================================
	///	UV Transform
	///===================================================
	Matrix4x4 uvTransformMatrix = MakeScaleMatrix(uvTransform.scale);
	uvTransformMatrix = Matrix4x4::Multiply(uvTransformMatrix, MakeRotateZMatrix(uvTransform.rotate.z));
	uvTransformMatrix = Matrix4x4::Multiply(uvTransformMatrix, MakeTranslateMatrix(uvTransform.translate));
	materialData_->uvTransform = uvTransformMatrix;
}

void Sprite::Draw(){
	commandList_->SetGraphicsRootDescriptorTable(3, TextureManager::GetInstance()->GetTextureSrvHandleGPU());
	commandList_->IASetVertexBuffers(0, 1, &vertexBufferViewSprite);
	commandList_->IASetIndexBuffer(&indexBufferView);
	commandList_->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
	commandList_->SetGraphicsRootConstantBufferView(1, transformResource_->GetGPUVirtualAddress());
	commandList_->DrawIndexedInstanced(6, 1, 0, 0, 0);
}

void Sprite::CreateBuffer(){
	//頂点用リソース
	vertexResource_ = CreateBufferResource(device_.Get(), sizeof(VertexData) * 4);
	//頂点バッファビューを作成	
	//リソースの先頭アドレスから使用する
	vertexBufferViewSprite.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	//使用するリソースsizeは頂点6つ分size
	vertexBufferViewSprite.SizeInBytes = sizeof(VertexData) * 4;
	//1頂点当たりのサイズ
	vertexBufferViewSprite.StrideInBytes = sizeof(VertexData);
	
	//transformtionMatrix用のリソース
	transformResource_ = CreateBufferResource(device_.Get(), sizeof(TransformationMatrix));
	//マテリアル用リソース
	materialResource_ = CreateBufferResource(device_.Get(), sizeof(Material));

	//index用リソース
	indexResource_ = CreateBufferResource(device_.Get(), sizeof(uint32_t) * 6);
	//リソースの先頭アドレスから使う
	indexBufferView.BufferLocation = indexResource_->GetGPUVirtualAddress();
	indexBufferView.SizeInBytes = sizeof(uint32_t) * 6;
	indexBufferView.Format = DXGI_FORMAT_R32_UINT;
}

void Sprite::Map(){
	IndexResourceMap();
	VertexResourceMap();
	TransformResourceMap();
	MaterialResourceMap();
}

void Sprite::IndexResourceMap(){
	uint32_t* indexData = nullptr;
	indexResource_->Map(0,nullptr,reinterpret_cast<void**>(&indexData));
	indexData[0] = 0; indexData[1] = 1; indexData[2] = 2;
	indexData[3] = 1; indexData[4] = 3; indexData[5] = 2;
	indexResource_->Unmap(0,nullptr);
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
	vertexData[2].normal = {0.0f,0.0f,-1.0f};
	
	vertexData[3].position = {640.0f,0.0f,0.0f,1.0f};//左下
	vertexData[3].texcoord = {1.0f,0.0f};
	vertexData[3].normal = {0.0f,0.0f,-1.0f};

	vertexResource_->Unmap(0, nullptr);
}

void Sprite::TransformResourceMap(){
	transformResource_->Map(0, nullptr, reinterpret_cast< void** >(&transformData));
	//単位行列を書き込んでおく
	*transformData = Matrix4x4::MakeIdentity();
	transformResource_->Unmap(0, nullptr);
}

void Sprite::MaterialResourceMap(){
	materialResource_->Map(0, nullptr, reinterpret_cast< void** >(&materialData_));
	materialData_->color = {1.0f,1.0f,1.0f,1.0f};
	materialData_->enableLighting = false;
	materialData_->uvTransform = Matrix4x4::MakeIdentity();
	materialResource_->Unmap(0, nullptr);
}



