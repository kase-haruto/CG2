#include "DemoParticle.h"

#include "lib/myFunc/MyFunc.h"
#include "lib/myFunc/Random.h"

#include "Engine/graphics/GraphicsGroup.h"


DemoParticle::DemoParticle(){

	ParticleSystem::SetName("demoParticle");

}

void DemoParticle::Initialize(const std::string& modelName, const std::string& texturePath){

	//50個性性
	particleNum_ = 10;
	emitter_.Initialize(particleNum_);

	ParticleSystem::Initialize(modelName, texturePath);

}

void DemoParticle::Update(){
	
	// 行動の更新
	PtlBehavior_Diffusion::ApplyBehavior(*this);

	// パーティクルが消えた分、新たに生成
	//if (particles_.size() < emitter_.count){
	//	ParticleSystem::Emit(emitter_.count - sizeof(particles_.size()));
	//}

	// 座標などの更新
	BaseParticle::Update();

}

