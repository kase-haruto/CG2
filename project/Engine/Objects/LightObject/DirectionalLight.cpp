#include <Engine/Objects/LightObject/DirectionalLight.h>

/* engine */
#include <Engine/Foundation/Utility/Func/MyFunc.h>
#include <Engine/Graphics/Device/DxCore.h>
#include <Engine/Graphics/Context/GraphicsGroup.h>
#include <Engine/Foundation/Json/JsonUtils.h>


/* externals */
#ifdef _DEBUG
#include<externals/imgui/imgui.h>
#endif // _DEBUG


DirectionalLight::DirectionalLight(const std::string& name){
	SceneObject::SetName(name, ObjectType::Light);
	ID3D12Device* device = GraphicsGroup::GetInstance()->GetDevice().Get();
	constantBuffer_.Initialize(device);
	configPath_ = "Resources/Configs/Engine/Objects/Light/DirectionalLightConfig/DirectionalLightConfig.json";
	LoadConfig(configPath_);
}

DirectionalLight::~DirectionalLight(){}

void DirectionalLight::Initialize() {}

void DirectionalLight::Update(){
	// データを更新
	DirectionalLightData data;
	data.color = config_.color;
	data.direction = config_.direction;
	data.intensity = config_.intensity;
	// バッファにデータを設定
	constantBuffer_.TransferData(data);
}

void DirectionalLight::SetCommand(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList,PipelineType type){
	Microsoft::WRL::ComPtr<ID3D12RootSignature>rootSignature_ = GraphicsGroup::GetInstance()->GetRootSignature(type);
	
	uint32_t index = 0;
	if (type == PipelineType::Object3D||PipelineType::SkinningObject3D){
		index = 4;
	}

	// ルートシグネチャをコマンドリストに設定する
	commandList->SetGraphicsRootSignature(rootSignature_.Get());
	constantBuffer_.SetCommand(commandList, index);
}

void DirectionalLight::ShowGui(){
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

	ImGui::SliderFloat3("direction", &config_.direction.x, -1.0f, 1.0f);
	ImGui::ColorEdit4("color", &config_.color.x);
	ImGui::SliderFloat("Intensity", &config_.intensity, 0.0f, 1.0f);
#endif // _DEBUG
}

 //===================================================================*/
 //                    config
 //===================================================================*/
void DirectionalLight::SaveConfig(const std::string& path) {
	JsonUtils::Save(path, config_);
}

void DirectionalLight::LoadConfig(const std::string& path) {
	JsonUtils::LoadOrCreate(path, config_);
}
