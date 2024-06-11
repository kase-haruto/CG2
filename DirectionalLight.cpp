#include "DirectionalLight.h"
#include"DirectXCommon.h"
#include"MyFunc.h"
#include"imgui.h"

DirectionalLight::DirectionalLight(){}

DirectionalLight::~DirectionalLight(){}

void DirectionalLight::Initialize(DirectXCommon* dxCommon){
	device_ = dxCommon->GetDevice();
	commandList_ = dxCommon->GetCommandList();
	rootSignature_ = dxCommon->GetRootSignature();

	CreateBuffer();
	Map();

	data_->color = {1.0f,1.0f,1.0f,1.0f};
	data_->direction = {0.0f,-1.0f,0.0f};
	data_->intensity = 1.0f;
}

void DirectionalLight::Update(){
	ImGui::Begin("directionalLight");
	ImGui::DragFloat3("direction",&data_->direction.x,0.01f);
	ImGui::ColorEdit4("color", &color_.x);
	ImGui::End();

	data_->color = color_;
}

void DirectionalLight::Render(){
	// ルートシグネチャをコマンドリストに設定する
	commandList_->SetGraphicsRootSignature(rootSignature_.Get());
	commandList_->SetGraphicsRootConstantBufferView(4, resource_->GetGPUVirtualAddress());
}

void DirectionalLight::CreateBuffer(){
	resource_ = CreateBufferResource(device_.Get(), sizeof(DirectionalLightData));
}

void DirectionalLight::Map(){
	resource_->Map(0, nullptr, reinterpret_cast< void** >(&data_));
	
}