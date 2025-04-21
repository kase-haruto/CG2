#include "DemoParticle.h"

#include "lib/myFunc/MyFunc.h"
#include "lib/myFunc/Random.h"

#include "Engine/graphics/GraphicsGroup.h"


DemoParticle::DemoParticle(){

	Particle::SetName("demoParticle");


}

void DemoParticle::Initialize(const std::string& modelName, const std::string& texturePath){

	//50個性性
	particleNum_ = 10;
	emitter_.Initialize(particleNum_);

	Particle::Initialize(modelName, texturePath);

}

void DemoParticle::Update(){
	
	// 座標などの更新
	BaseParticle::Update();

}

