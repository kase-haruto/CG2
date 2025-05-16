#include <Engine/Objects/LightObject/PointLight.h>

/* engine */
#include <Engine/Foundation/Utility/Func/MyFunc.h>
#include <Engine/Graphics/Device/DxCore.h>
#include <Engine/Graphics/Context/GraphicsGroup.h>
#include <Engine/Foundation/Json/JsonUtils.h>

#ifdef _DEBUG
#include<externals/imgui/imgui.h>
#endif // _DEBUG



PointLight::PointLight(const std::string& name){
	SceneObject::SetName(name, ObjectType::Light);
}

PointLight::~PointLight(){}

void PointLight::Initialize(){
	ID3D12Device* device = GraphicsGroup::GetInstance()->GetDevice().Get();
	constantBuffer_.Initialize(device);

	configPath_ = "Resources/Configs/Engine/Objects/Light/PointLightConfig/PointLightConfig.json";
	LoadConfig(configPath_);
}

void PointLight::Update(){
	
	PointLightData data;
	data.color = config_.color;
	data.position = config_.position;
	data.intensity = config_.intensity;
	data.radius = config_.radius;
	data.decay = config_.decay;
	constantBuffer_.TransferData(data);
}

void PointLight::ShowGui(){
#ifdef _DEBUG
	ImGui::Dummy(ImVec2(0.0f, 5.0f));
	if (ImGui::Button("SaveConfig")) {
		SaveConfig(configPath_);
	}
	ImGui::SameLine();
	if (ImGui::Button("LoadConfig")) {
		LoadConfig(configPath_);
	}
	ImGui::Separator();

	ImGui::Separator();
	ImGui::DragFloat3("position", &config_.position.x, 0.01f);
	ImGui::ColorEdit4("color", &config_.color.x);
	ImGui::SliderFloat("Intensity", &config_.intensity, 0.0f, 1.0f);
	ImGui::DragFloat("radius", &config_.radius, 0.01f);
	ImGui::DragFloat("decay", &config_.decay, 0.01f);
#endif // _DEBUG


}

void PointLight::SetCommand(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList, PipelineType type){
	Microsoft::WRL::ComPtr<ID3D12RootSignature>rootSignature_ = GraphicsGroup::GetInstance()->GetRootSignature(type);
	uint32_t index = 0;
	if (type == PipelineType::Object3D || PipelineType::SkinningObject3D){
		index = 6;
	}
	// ルートシグネチャをコマンドリストに設定する
	commandList->SetGraphicsRootSignature(rootSignature_.Get());
	constantBuffer_.SetCommand(commandList, index);
}

 //===================================================================*/
 //                    config
 //===================================================================*/
void PointLight::SaveConfig(const std::string& path) {
	JsonUtils::Save(path, config_);
}

void PointLight::LoadConfig(const std::string& path) {
	JsonUtils::LoadOrCreate(path, config_);
}
