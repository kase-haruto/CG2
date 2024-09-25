#include "PointLight.h"
#include"DirectXCommon.h"
#include"MyFunc.h"
#include"imgui.h"

PointLight::PointLight(){}

void PointLight::Initialize(DirectXCommon* dxCommon){
	device_ = dxCommon->GetDevice();
	commandList_ = dxCommon->GetCommandList();

	CreateBuffer();
	Map();

	data_->color = {1.0f,1.0f,1.0f,1.0f};
	data_->position = {0.0f,-1.0f,0.0f};
	data_->intensity = 1.0f;
}

void PointLight::Update(){

}

void PointLight::Render(){
	assert(rootSignature_);
	ImGui::Begin("PointLight");
	ImGui::DragFloat3("position", &data_->position.x, 0.01f);
	ImGui::ColorEdit4("color", &data_->color.x); // color_ではなく、data_->colorを直接操作
	ImGui::DragFloat("Intensity", &data_->intensity, 0.01f);
	ImGui::End();

	// ルートシグネチャをコマンドリストに設定する
	commandList_->SetGraphicsRootSignature(rootSignature_.Get());
	commandList_->SetGraphicsRootConstantBufferView(6, resource_->GetGPUVirtualAddress());
}

void PointLight::CreateBuffer(){
	resource_ = CreateBufferResource(device_.Get(), sizeof(PointLightData));
}

void PointLight::Map(){
	resource_->Map(0, nullptr, reinterpret_cast< void** >(&data_));

}

void PointLight::SetRootSignature(const Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature){
	rootSignature_ = rootSignature;
}