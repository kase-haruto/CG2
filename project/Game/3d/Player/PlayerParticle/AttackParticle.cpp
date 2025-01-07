#include "AttackParticle.h"

#include "Engine/core/System.h"
#include "Engine/core/Math/Ease.h"

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
    isFixationAlpha_ = false;

    // ランダムスケールを使用する
    SetUseRandomScale(true);
	SetRandomScaleRange(0.5f, 1.0f); // ランダム値の範囲を指定

}


void AttackParticle::Update(){
    if (behavior_){
        behavior_->ApplyBehavior(*this);
    }

    for (auto& particle : particles_){
        // スケールのイージング処理
        float progress = particle.currentTime / particle.lifeTime; // 0.0 ~ 1.0
        progress = std::clamp(progress, 0.0f, 1.0f); // 安全にクランプ
        float minScale = 0.5f; // 最小スケール
        float easedScale = minScale + (particle.maxScale - minScale) * EvoEase::EaseOutQuad(progress);
        particle.transform.scale = Vector3(easedScale, easedScale, easedScale);
    }

    BaseParticle::Update();
}