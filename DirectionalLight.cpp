#include "DirectionalLight.h"
#include"DirectXCommon.h"
#include"MyFunc.h"

DirectionalLight::DirectionalLight(){}

DirectionalLight::~DirectionalLight(){}

void DirectionalLight::Initialize(DirectXCommon* dxCommon){
	device_ = dxCommon->GetDevice();
	commandList_ = dxCommon->GetCommandList();

	CreateBuffer();
	Map();
	data_->color = {1.0f,1.0f,1.0f,1.0f};
	data_->direction = {0.0f,-1.0f,0.0f};
	data_->intensity = 1.0f;
}

void DirectionalLight::Render(){
	//commandList_->SetGraphicsRootConstantBufferView(5, resource_->GetGPUVirtualAddress());
}

void DirectionalLight::CreateBuffer(){
	resource_ = CreateBufferResource(device_.Get(), sizeof(DirectionalLightData));
}

void DirectionalLight::Map(){
	resource_->Map(0, nullptr, reinterpret_cast< void** >(&data_));
}