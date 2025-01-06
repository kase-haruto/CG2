#include "TornadoParticle.h"

#include "particleBehavior/PtlBehavior_Tornado.h"

TornadoParticle::TornadoParticle(){

	ParticleSystem::SetName("tornadoParticle");

	behavior_ = std::make_unique<PtlBehavior_Tornado>(emitter_.transform.translate,1.2f,0.9f);

	emitPosX_ = true;
	emitNegX_ = true;
	emitPosY_ = false;
	emitNegY_ = false;
	emitPosZ_ = true;
	emitNegZ_ = true;

	autoEmit_ = false;
}

void TornadoParticle::Initialize(const std::string& modelName, const std::string& texturePath){

	particleNum_ = 0;
	emitter_.Initialize(particleNum_);

	ParticleSystem::Initialize(modelName,texturePath);

}

void TornadoParticle::Update(){

	// 行動の更新
	if (behavior_){
		behavior_->ApplyBehavior(*this);
	}

	BaseParticle::Update();

}
