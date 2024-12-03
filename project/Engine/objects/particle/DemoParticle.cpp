#include "DemoParticle.h"

#include "lib/myFunc/Random.h"

#include "Engine/graphics/GraphicsGroup.h"


DemoParticle::DemoParticle(){

	ParticleSystem::SetName("demoParticle");

}

void DemoParticle::Initialize(const std::string& modelName){

	//50個性性
	emitter_.Initialize(50);

	ParticleSystem::Initialize(modelName);

}

void DemoParticle::Update(){
	
	// 行動の更新
	PtlBehavior_Diffusion::ApplyBehavior(*this);

	// 座標などの更新
	BaseParticle::Update();

}

