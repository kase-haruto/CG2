#include "ParticleSystemObject.h"

#include <Engine/Foundation/Clock/ClockManager.h>
#include <Engine/System/Command/EditorCommand/GuiCommand/ImGuiHelper/GuiCmd.h>


ParticleSystemObject::ParticleSystemObject(const std::string& name){
	SceneObject::SetName(name, ObjectType::ParticleSystem);
}

void ParticleSystemObject::Update(){

	position_ = worldTransform_.GetWorldPosition();

	worldTransform_.Update();

	FxEmitter::Update();
}

void ParticleSystemObject::ShowGui(){
	//particle 名
	ImGui::Text("Particle System: %s", name_.c_str());
	worldTransform_.ShowImGui("transform 3d");
	ImGui::Separator();

	FxEmitter::ShowGui();
}
