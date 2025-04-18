#include "DemoParticle.h"

#include "lib/myFunc/MyFunc.h"
#include "lib/myFunc/Random.h"

#include "Engine/graphics/GraphicsGroup.h"


DemoParticle::DemoParticle(){

	Particle::SetName("demoParticle");

	behavior_ = std::make_unique<PtlBehavior_Diffusion>();

}

void DemoParticle::Initialize(const std::string& modelName, const std::string& texturePath){

	//50個性性
	particleNum_ = 10;
	emitter_.Initialize(particleNum_);

	Particle::Initialize(modelName, texturePath);

}

void DemoParticle::Update(){
	
	// 行動の更新
	if (behavior_){
		behavior_->ApplyBehavior(*this);
	}

	// パーティクルが消えた分、新たに生成
	//if (particles_.size() < emitter_.count){
	//	Particle::Emit(emitter_.count - sizeof(particles_.size()));
	//}

	// 座標などの更新
	BaseParticle::Update();

}

