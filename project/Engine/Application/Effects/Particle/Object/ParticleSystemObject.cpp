#include "ParticleSystemObject.h"

#include <Engine/Foundation/Clock/ClockManager.h>
#include <Engine/System/Command/EditorCommand/GuiCommand/ImGuiHelper/GuiCmd.h>


ParticleSystemObject::ParticleSystemObject(const std::string& name){
	SceneObject::SetName(name, ObjectType::ParticleSystem);
}

void ParticleSystemObject::Update(){
	worldTransform_.Update();
	position_ = worldTransform_.GetWorldPosition();
	FxEmitter::Update();
}

void ParticleSystemObject::ShowGui(){
	FxEmitter::ShowGui();
}

void ParticleSystemObject::SetDrawEnable(bool isDrawEnable){
	FxEmitter::SetDrawEnable(isDrawEnable);
}

void ParticleSystemObject::ApplyConfig(){
	FxEmitter::ApplyConfigFrom(static_cast< const EmitterConfig& >(config_));
	name_ = config_.name;
	id_ = config_.guid;
	parentId_ = config_.parentGuid;
	worldTransform_.ApplyConfig(config_.transform);
}

void ParticleSystemObject::ExtractConfig(){
	FxEmitter::ExtractConfigTo(static_cast< EmitterConfig& >(config_));
	config_.name = name_;
	config_.guid = id_;
	config_.parentGuid = parentId_;
	worldTransform_.ExtractConfig();
}
