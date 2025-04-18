#include "PointLight.h"

/* engine */
#include "lib/myFunc/MyFunc.h"
#include "engine/core/DirectX/DxCore.h"
#include "engine/graphics/GraphicsGroup.h"

#ifdef _DEBUG
#include<externals/imgui/imgui.h>
#endif // _DEBUG


PointLight::PointLight(){}

PointLight::~PointLight(){}

void PointLight::Initialize(const DxCore* dxCore){
	pDxCore_ = dxCore;
	name_ = "PointLight";
	objectType_ = ObjectType::Light;
	CreateBuffer();
	Map();
}

void PointLight::Update(){

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
	data_->radius = 20.0f;
	data_->decay = 1.0f;
	resource_->Unmap(0, nullptr);
}

void PointLight::ShowGui(){
	SceneObject::ShowGui();

	ImGui::DragFloat3("position", &data_->position.x, 0.01f);
	ImGui::ColorEdit4("color", &data_->color.x); // color_ではなく、data_->colorを直接操作
	ImGui::SliderFloat("Intensity", &data_->intensity, 0.0f, 1.0f);
	ImGui::DragFloat("radius", &data_->radius, 0.01f);
	ImGui::DragFloat("decay", &data_->decay, 0.01f);
}

void PointLight::SetCommand(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList, PipelineType type){
	Microsoft::WRL::ComPtr<ID3D12RootSignature>rootSignature_ = GraphicsGroup::GetInstance()->GetRootSignature(type);
	uint32_t index = 0;
	if (type == PipelineType::Object3D || PipelineType::SkinningObject3D){
		index = 6;
	}
	// ルートシグネチャをコマンドリストに設定する
	commandList->SetGraphicsRootSignature(rootSignature_.Get());
	commandList->SetGraphicsRootConstantBufferView(index, resource_->GetGPUVirtualAddress());
}

void PointLight::SetPosition(const Vector3& position){
	data_->position = position;
}
