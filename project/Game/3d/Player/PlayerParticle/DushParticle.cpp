#include "DushParticle.h"

#include "lib/myFunc/MyFunc.h"
#include "lib/myFunc/Random.h"

#include "Engine/objects/particle/particleBehavior/PtlBehavior_Diffusion.h"

#include "Engine/graphics/GraphicsGroup.h"

DushParticle::DushParticle(){

	ParticleSystem::SetName("demoParticle");

	behavior_ = std::make_unique<PtlBehavior_Diffusion>();

	isBillboard_ = true;

}

void DushParticle::Initialize(const std::string& modelName, const std::string& texturePath){
	particleNum_ = 10;
	emitter_.Initialize(particleNum_);

	ParticleSystem::Initialize(modelName, texturePath);
}

void DushParticle::Update(){

	// 行動の更新
	if (behavior_){
		behavior_->ApplyBehavior(*this);
	}

	// 座標などの更新
	BaseParticle::Update();

}
