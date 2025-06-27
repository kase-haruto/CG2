#include "ParticleSystemObject.h"

#include <Engine/Foundation/Clock/ClockManager.h>
#include <Engine/System/Command/EditorCommand/GuiCommand/ImGuiHelper/GuiCmd.h>


ParticleSystemObject::ParticleSystemObject(const std::string& name){
	SceneObject::SetName(name, ObjectType::ParticleSystem);
}

void ParticleSystemObject::Update(){
	FxEmitter::Update();
}

void ParticleSystemObject::ShowGui(){
	FxEmitter::ShowGui();
}

void ParticleSystemObject::SetDrawEnable(bool isDrawEnable){
	FxEmitter::SetDrawEnable(isDrawEnable);
}

void ParticleSystemObject::ApplyConfig(){
	FxEmitter::ApplyConfig();
}

void ParticleSystemObject::ExtractConfig(){
	FxEmitter::ExtractConfig();
}
