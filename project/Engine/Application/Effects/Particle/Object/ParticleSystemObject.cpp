#include "ParticleSystemObject.h"

#include <Engine/Foundation/Clock/ClockManager.h>
#include <Engine/System/Command/EditorCommand/GuiCommand/ImGuiHelper/GuiCmd.h>
ParticleSystemObject::ParticleSystemObject(const std::string& name){
	SceneObject::SetName(name, ObjectType::ParticleSystem);
}

void ParticleSystemObject::Update(){
	particleEmitter_->Update(ClockManager::GetInstance()->GetDeltaTime());
}

void ParticleSystemObject::ShowGui(){
	GuiCmd::DragFloat3("Position", particleEmitter_->position_, 0.1f, -100.0f, 100.0f);
	GuiCmd::DragFloat("Emit Rate", particleEmitter_->emitRate_, 0.01f, 0.0f, 10.0f);
}
