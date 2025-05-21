#include <Engine/Objects/LightObject/DirectionalLight.h>

/* engine */
#include <Engine/Foundation/Utility/Func/MyFunc.h>
#include <Engine/Graphics/Device/DxCore.h>
#include <Engine/Graphics/Context/GraphicsGroup.h>
#include <Engine/Foundation/Json/JsonUtils.h>
#include <Engine/foundation/Utility/FileSystem/ConfigPathResolver/ConfigPathResolver.h>

/* externals */
#ifdef _DEBUG
#include<externals/imgui/imgui.h>
#endif // _DEBUG


DirectionalLight::DirectionalLight(const std::string& name){
	SceneObject::SetName(name, ObjectType::Light);


	ID3D12Device* device = GraphicsGroup::GetInstance()->GetDevice().Get();
	constantBuffer_.Initialize(device);

	// コンフィグパスの生成 preset名はdefault
	SceneObject::SetConfigPath(ConfigPathResolver::ResolvePath(GetObjectTypeName(), GetName()));
	//コンフィグの適用
	LoadConfig(configPath_);
}

DirectionalLight::~DirectionalLight(){}

void DirectionalLight::Initialize() {}

void DirectionalLight::Update(){
	ApplyConfig();
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

	if (ImGui::Button("Save Config")) {
		SaveConfig(GetConfigPath());
	}
	ImGui::SameLine();
	if (ImGui::Button("Load Config")) {
		LoadConfig(GetConfigPath());
	}

	ImGui::Separator();

	ImGui::SliderFloat3("direction", &config_.direction.x, -1.0f, 1.0f);
	ImGui::ColorEdit4("color", &config_.color.x);
	ImGui::SliderFloat("Intensity", &config_.intensity, 0.0f, 1.0f);
#endif // _DEBUG
}

void DirectionalLight::ApplyConfig() {
		// config_ → GPU転送用データに詰め替え
	DirectionalLightData data;
	data.color = config_.color;
	data.direction = config_.direction;
	data.intensity = config_.intensity;

	// 定数バッファへ
	constantBuffer_.TransferData(data);
}

