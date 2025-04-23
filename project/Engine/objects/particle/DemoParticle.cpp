#include "DemoParticle.h"

#include "lib/myFunc/MyFunc.h"
#include "lib/myFunc/Random.h"

#include "Engine/graphics/GraphicsGroup.h"


DemoParticle::DemoParticle(){

	Particle::SetName("demoParticle");


}

void DemoParticle::Initialize(const std::string& modelName, const std::string& texturePath, int32_t count = 1){

	//50個性性
	particleNum_ = 10;

	Particle::Initialize(modelName, texturePath, count);

}

void DemoParticle::Update(){
	
	// 座標などの更新
	BaseParticle::Update();

}

