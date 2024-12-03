#include "DemoParticle.h"

#include "lib/myFunc/Random.h"

#include "Engine/graphics/GraphicsGroup.h"

void DemoParticle::Initialize(const std::string& modelName, const uint32_t count ){

	// リソースの生成
	BaseParticle::Initialize(modelName,count);


}

void DemoParticle::Update(){
	
	
	// 行動の更新
	PtlBehavior_Diffusion::ApplyBehavior(*this);

	// 座標などの更新
	BaseParticle::Update();

}

void DemoParticle::Draw(){

	// particleの描画
	BaseParticle::Draw();

}

void DemoParticle::Emit(uint32_t count){

	// 初期化(50個生成
	emitter_.Initialize(count);

	BaseParticle::Emit(count);

}
