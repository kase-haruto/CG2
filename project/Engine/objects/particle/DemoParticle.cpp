#include "DemoParticle.h"

#include "lib/myFunc/Random.h"

#include "Engine/graphics/GraphicsGroup.h"

#include "ParticleManager.h"


DemoParticle::DemoParticle(){

	ParticleSystem::SetName("demoParticle");

}

void DemoParticle::Initialize(const std::string& modelName, const std::string& textureFilePath, uint32_t count){

	// 親クラス(ParticleSystem)の初期化を呼び出す
	ParticleSystem::Initialize(modelName, textureFilePath, count);

	ParticleManager::GetInstance()->CreateParticleGroup("Demo", textureFilePath);

}

void DemoParticle::Update(){
	
	// 行動の更新
	PtlBehavior_Diffusion::ApplyBehavior(*this);

	// 座標などの更新
	BaseParticle::Update();

}

