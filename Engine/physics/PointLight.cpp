#include "PointLight.h"
#include"myfunc/MyFunc.h"
#include "core/DirectX/DxCore.h"

#ifdef _DEBUG
#include"imgui.h"
#endif // _DEBUG


PointLight::PointLight(){}

PointLight::~PointLight(){}

void PointLight::Initialize(const DxCore* dxCore){
	pDxCore_ = dxCore;
	CreateBuffer();
	Map();

	
}

void PointLight::Update(){

}

void PointLight::Render(){
	assert(rootSignature_);
#ifdef _DEBUG
	ImGui::Begin("PointLight");
	ImGui::DragFloat3("position", &data_->position.x, 0.01f);
	ImGui::ColorEdit4("color", &data_->color.x); // color_ではなく、data_->colorを直接操作
	ImGui::SliderFloat("Intensity", &data_->intensity, 0.0f, 1.0f);
	ImGui::DragFloat("radius", &data_->radius, 0.01f);
	ImGui::DragFloat("decay", &data_->decay, 0.01f);
	ImGui::End();
#endif // _DEBUG



	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList = pDxCore_->GetCommandList();

	// ルートシグネチャをコマンドリストに設定する
	commandList->SetGraphicsRootSignature(rootSignature_.Get());
	commandList->SetGraphicsRootConstantBufferView(6, resource_->GetGPUVirtualAddress());
}

void PointLight::CreateBuffer(){
	Microsoft::WRL::ComPtr<ID3D12Device> device = pDxCore_->GetDevice();

	resource_ = CreateBufferResource(device, sizeof(PointLightData));
}

void PointLight::Map(){
	resource_->Map(0, nullptr, reinterpret_cast< void** >(&data_));
	data_->color = {1.0f,1.0f,1.0f,1.0f};
	data_->position = {0.0f,2.0f,0.0f};
	data_->intensity = 1.0f;
	data_->radius = 4.0f;
	data_->decay = 1.0f;
	resource_->Unmap(0, nullptr);
}

void PointLight::SetRootSignature(const Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature){
	rootSignature_ = rootSignature;
}