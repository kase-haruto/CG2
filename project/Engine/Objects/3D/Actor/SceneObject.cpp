#include "SceneObject.h"
#include "SceneObjectManager.h"
#include <Engine/graphics/Camera/Manager/CameraManager.h>
#include <Engine/Foundation/Utility/Func/MyFunc.h>
#include <Engine/Foundation/Json/JsonUtils.h>

#include <externals/imgui/imgui.h>
#include <externals/imgui/ImGuizmo.h>

static const char* ObjectTypeToString(ObjectType type) {
	switch (type) {
		case ObjectType::Camera:    return "Camera";
		case ObjectType::Light:     return "Light";
		case ObjectType::GameObject:return "GameObject";
		default:                    return "None";
	}
}

SceneObject::SceneObject(){

	//リストに追加

	worldTransform_.Initialize();
}

void SceneObject::ShowGui(){
	ImGui::Dummy(ImVec2(0.0f, 5.0f));
	ImGui::Separator();

	if (ImGui::Button("SaveConfig")) {
		SaveConfig(configPath_);
	}
	ImGui::SameLine();
	if (ImGui::Button("LoadConfig")) {
		LoadConfig(configPath_);
	}
	worldTransform_.ShowImGui("world");

}

std::string SceneObject::GetObjectTypeName() const {
	return ObjectTypeToString(objectType_);
}

void SceneObject::SetName(const std::string& name, ObjectType type){
	name_ = name;
	objectType_ = type;
}

void SceneObject::LoadConfig(const std::string& path) {
	nlohmann::json j;
	if (JsonUtils::Load(path, j)) {
		ApplyConfigFromJson(j);
	}
}

void SceneObject::SaveConfig(const std::string& path) const {
	nlohmann::json j;
	ExtractConfigToJson(j);
	JsonUtils::Save(path, j);
}