#include <Engine/Objects/LightObject/PointLight.h>

/* engine */
#include <Engine/Foundation/Utility/Func/MyFunc.h>
#include <Engine/Graphics/Device/DxCore.h>
#include <Engine/Graphics/Context/GraphicsGroup.h>
#include <Engine/Foundation/Json/JsonUtils.h>
#include <Engine/foundation/Utility/FileSystem/ConfigPathResolver/ConfigPathResolver.h>
#include <Engine/System/Command/EditorCommand/GuiCommand/ImGuiHelper/GuiCmd.h>

#ifdef _DEBUG
#include<externals/imgui/imgui.h>
#endif // _DEBUG



PointLight::PointLight(const std::string& name){
	SceneObject::SetName(name, ObjectType::Light);

	ID3D12Device* device = GraphicsGroup::GetInstance()->GetDevice().Get();
	constantBuffer_.Initialize(device);

	SceneObject::SetConfigPath(ConfigPathResolver::ResolvePath(GetObjectTypeName(), GetName()));
	LoadConfig(configPath_);

	isEnableRaycast_ = false;
}

PointLight::~PointLight(){}

void PointLight::Initialize(){}

void PointLight::Update(){
	ApplyConfig();
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
	GuiCmd::DragFloat3("position", config_.position );
	GuiCmd::ColorEdit4("color",config_.color);
	ImGui::SliderFloat("Intensity", &config_.intensity, 0.0f, 1.0f);
	GuiCmd::DragFloat("radius", config_.radius);
	GuiCmd::DragFloat("decay", config_.decay);
#endif // _DEBUG


}

void PointLight::SetCommand(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList, PipelineType type){
	uint32_t index = 0;
	if (type == PipelineType::Object3D || PipelineType::SkinningObject3D){
		index = 5;
	}
	constantBuffer_.SetCommand(commandList, index);
}

//===================================================================*/
 //                    config
 //===================================================================*/
void PointLight::ApplyConfig() {
	PointLightData data;
	data.color = config_.color;
	data.position = config_.position;
	data.intensity = config_.intensity;
	data.radius = config_.radius;
	data.decay = config_.decay;
	constantBuffer_.TransferData(data);
}


