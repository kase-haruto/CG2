#include "HitParticle.h"

#include "Engine/core/System.h"
#include "Engine/core/Math/Ease.h"

HitParticle::HitParticle(){}

void HitParticle::Initialize(const std::string& modelName, const std::string& texturePath){
	ParticleSystem::SetName("HitParticle");
	particleNum_ = 0;
	emitter_.Initialize(particleNum_);
	behavior_ = std::make_unique<PtlBehavior_Diffusion>();
	ParticleSystem::Initialize(modelName, texturePath);
	isStatic_ = false;
	autoEmit_ = false;
}

void HitParticle::Update(){
    if (behavior_){
        behavior_->ApplyBehavior(*this);
    }

    // サイズ変更と回転ロジックを追加
    for (auto& particle : particles_){
        // イージングでスケールを変更
        float progress = particle.currentTime / particle.lifeTime; // 0.0 ~ 1.0
        progress = std::clamp(progress, 0.0f, 1.0f); // 安全にクランプ
        float minScale = 0.1f; // 最小スケール
        float maxScale = 1.0f; // 初期スケール
        float easedScale = maxScale - (maxScale - minScale) * EvoEase::EaseOutQuad(progress);
        particle.transform.scale = Vector3(easedScale, easedScale, easedScale);
    }

    BaseParticle::Update();
}

Vector3 HitParticle::GenerateVelocity(float speed){
    // デフォルトのランダムな方向の速度生成
    Vector3 velocity = Vector3(1.);
    return velocity * speed;
}
