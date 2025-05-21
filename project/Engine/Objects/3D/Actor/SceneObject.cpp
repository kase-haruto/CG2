#include "SceneObject.h"
#include "SceneObjectManager.h"
#include <Engine/graphics/Camera/Manager/CameraManager.h>
#include <Engine/Foundation/Utility/Func/MyFunc.h>

#include <externals/imgui/imgui.h>
#include <externals/imgui/ImGuizmo.h>

SceneObject::~SceneObject(){
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

void SceneObject::EnableGuiList() {
}

void SceneObject::SetName(const std::string& name, ObjectType type){
	name_ = name;
	objectType_ = type;
}
