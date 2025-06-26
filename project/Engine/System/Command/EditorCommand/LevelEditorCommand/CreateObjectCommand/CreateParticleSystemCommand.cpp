#include "CreateParticleSystemCommand.h"
// engine
#include <Engine/Scene/Context/SceneContext.h>
#include <Engine/Application/Effects/FxSystem.h>
#include <Engine/Application/Effects/Particle/Object/ParticleSystemObject.h>

CreateParticleSystemObjectCommand::CreateParticleSystemObjectCommand(SceneContext* context, ObjectFactory factory)
	:context_(context), factory_(std::move(factory)){}

void CreateParticleSystemObjectCommand::Execute(){
	auto obj = factory_();
	particleSystem_ = obj.get();

	// 所有権はcontextに渡す
	context_->AddEditorObject<ParticleSystemObject>(std::move(obj));

	// emitterとして登録
	context_->GetFxSystem()->AddEmitter(particleSystem_);
}

void CreateParticleSystemObjectCommand::Undo(){
	if (particleSystem_){
		context_->GetFxSystem()->RemoveEmitter(particleSystem_);
		context_->RemoveEditorObject(particleSystem_);
		particleSystem_ = nullptr;
	}
}

const char* CreateParticleSystemObjectCommand::GetName() const{
	return name_.c_str();
}
