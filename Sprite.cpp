﻿#include "Sprite.h"
#include"DirectXCommon.h"
#include"MyFunc.h"
#include"VertexData.h"
#include"imgui.h"

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
	commandList_->IASetVertexBuffers(0, 1, &vertexBufferViewSprite);
	commandList_->SetGraphicsRootConstantBufferView(1, transformResource_->GetGPUVirtualAddress());
	commandList_->DrawInstanced(6, 1, 0, 0);
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
}

void Sprite::Map(){
	VertexResourceMap();
	TransformResourceMap();
}

void Sprite::VertexResourceMap(){
	VertexData* vertexData = nullptr;
	vertexResource_->Map(0, nullptr, reinterpret_cast< void** >(&vertexData));

	//1枚目の三角形
	vertexData[0].position = {0.0f,360.0f,0.0f,1.0f};//左下
	vertexData[0].texcoord = {0.0f,1.0f};
	vertexData[1].position = {0.0f,0.0f,0.0f,1.0f};//左下
	vertexData[1].texcoord = {0.0f,0.0f};
	vertexData[2].position = {640.0f,360.0f,0.0f,1.0f};//左下
	vertexData[2].texcoord = {1.0f,1.0f};
	//2枚目の三角形
	vertexData[3].position = {0.0f,0.0f,0.0f,1.0f};//左下
	vertexData[3].texcoord = {0.0f,0.0f};
	vertexData[4].position = {640.0f,0.0f,0.0f,1.0f};//左下
	vertexData[4].texcoord = {1.0f,0.0f};
	vertexData[5].position = {640.0f,360.0f,0.0f,1.0f};//左下
	vertexData[5].texcoord = {1.0f,1.0f};

}

void Sprite::TransformResourceMap(){
	transformResource_->Map(0, nullptr, reinterpret_cast< void** >(&transformData));
	//単位行列を書き込んでおく
	*transformData = Matrix4x4::MakeIdentity();
}

