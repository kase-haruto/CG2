#include "DirectionalLight.h"

/* engine */
#include "lib/myFunc/MyFunc.h"
#include "engine/core/DirectX/DxCore.h"
#include "engine/graphics/GraphicsGroup.h"


/* externals */
#ifdef _DEBUG
#include<externals/imgui/imgui.h>
#endif // _DEBUG


DirectionalLight::DirectionalLight(){}

DirectionalLight::~DirectionalLight(){}

void DirectionalLight::Initialize(const DxCore* dxCore){
	pDxCore_ = dxCore;
	name_ = "DirectionalLight";
	objectType_ = ObjectType::Light;
	CreateBuffer();
	Map();
}


void DirectionalLight::Update(){
	
}


void DirectionalLight::CreateBuffer(){
	Microsoft::WRL::ComPtr<ID3D12Device> device = pDxCore_->GetDevice();

	resource_ = CreateBufferResource(device, sizeof(DirectionalLightData));
}

void DirectionalLight::Map(){
	resource_->Map(0, nullptr, reinterpret_cast< void** >(&data_));
	data_->color = {1.0f,1.0f,1.0f,1.0f};
	data_->direction = {0.0f,-1.0f,0.0f};
	data_->intensity = 0.25f;
	resource_->Unmap(0, nullptr);
	
}

void DirectionalLight::SetCommand(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList,PipelineType type){
	Microsoft::WRL::ComPtr<ID3D12RootSignature>rootSignature_ = GraphicsGroup::GetInstance()->GetRootSignature(type);
	
	uint32_t index = 0;
	if (type == PipelineType::Object3D||PipelineType::SkinningObject3D){
		index = 4;
	}

	// ルートシグネチャをコマンドリストに設定する
	commandList->SetGraphicsRootSignature(rootSignature_.Get());
	commandList->SetGraphicsRootConstantBufferView(index, resource_->GetGPUVirtualAddress());
}


void DirectionalLight::ShowGui(){
	SceneObject::ShowGui();

	ImGui::SliderFloat3("direction", &data_->direction.x, -1.0f, 1.0f);
	ImGui::ColorEdit4("color", &data_->color.x); // color_ではなく、data_->colorを直接操作
	ImGui::SliderFloat("Intensity", &data_->intensity, 0.0f, 1.0f);
}

