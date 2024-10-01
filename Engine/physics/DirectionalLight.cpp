#include "DirectionalLight.h"
#include"DirectXCommon.h"
#include"MyFunc.h"
#include"imgui.h"

DirectionalLight::DirectionalLight(){}

DirectionalLight::~DirectionalLight(){
	device_.Reset();
	resource_->Release();
}



void DirectionalLight::Initialize(DirectXCommon* dxCommon){
	device_ = dxCommon->GetDevice();
	commandList_ = dxCommon->GetCommandList();

	CreateBuffer();
	Map();

	
}

void DirectionalLight::Update(){
	
}

void DirectionalLight::Render(){
	assert(rootSignature_);
	ImGui::Begin("directionalLight");
	ImGui::SliderFloat3("direction", &data_->direction.x, -1.0f, 1.0f);
	ImGui::ColorEdit4("color", &data_->color.x); // color_ではなく、data_->colorを直接操作
	ImGui::SliderFloat("Intensity", &data_->intensity, 0.0f,1.0f);
	ImGui::End();

	// ルートシグネチャをコマンドリストに設定する
	commandList_->SetGraphicsRootSignature(rootSignature_.Get());
	commandList_->SetGraphicsRootConstantBufferView(4, resource_->GetGPUVirtualAddress());
}


void DirectionalLight::CreateBuffer(){
	resource_ = CreateBufferResource(device_.Get(), sizeof(DirectionalLightData));
}

void DirectionalLight::Map(){
	resource_->Map(0, nullptr, reinterpret_cast< void** >(&data_));
	data_->color = {1.0f,1.0f,1.0f,1.0f};
	data_->direction = {0.0f,-1.0f,0.0f};
	data_->intensity = 1.0f;
	resource_->Unmap(0, nullptr);
	
}

void DirectionalLight::SetRootSignature(const Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature){
	rootSignature_ = rootSignature;
}