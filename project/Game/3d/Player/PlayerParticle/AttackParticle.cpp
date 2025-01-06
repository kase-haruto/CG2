#include "AttackParticle.h"

AttackParticle::AttackParticle(){

}

void AttackParticle::Initialize(const std::string& modelName, 
								const std::string& texturePath){
	ParticleSystem::SetName("AttackParticle");
	particleNum_ = 0;
	emitter_.Initialize(particleNum_);

	behavior_ = std::make_unique<PtlBehavior_Diffusion>();

	ParticleSystem::Initialize(modelName, texturePath);

	isStatic_ = true;
	autoEmit_ = false;

}

void AttackParticle::Update(){
	if (behavior_){
		behavior_->ApplyBehavior(*this);
	}

	BaseParticle::Update();
}
