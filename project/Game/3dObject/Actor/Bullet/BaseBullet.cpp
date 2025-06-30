#include "BaseBullet.h"
/* ========================================================================
/* include space
/* ===================================================================== */

/* engine */
#include <Engine/Foundation/Clock/ClockManager.h>
#include <Engine/Objects/Collider/BoxCollider.h>
#include <Engine/Application/Effects/Intermediary/FxIntermediary.h>
/* external */
#include <externals/imgui/imgui.h>

BaseBullet::BaseBullet(const std::string& modelName, const std::string& name)
	:Actor::Actor(modelName, name){
	collider_->SetType(ColliderType::Type_PlayerAttack);
	collider_->SetTargetType(ColliderType::Type_Enemy);
	collider_->SetOwner(this);
	collider_->SetIsDrawCollider(false);

	const std::string path = "Resources/Assets/Configs/Effect/";
	trailFx_ = std::make_unique<FxEmitter>();
	trailFx_->LoadConfig(path + "BulletTrail.json");
	FxIntermediary::GetInstance()->Attach(trailFx_.get());
}

BaseBullet::~BaseBullet() {
	
}

/////////////////////////////////////////////////////////////////////////////////////////
//		初期化
/////////////////////////////////////////////////////////////////////////////////////////
void BaseBullet::ShootInitialize(const Vector3 initPos, const Vector3 velocity) {
	worldTransform_.translation = initPos;
	velocity_ = velocity;
	moveSpeed_ = 15.0f;
	isAlive_ = true;
	OnShot();
}

/////////////////////////////////////////////////////////////////////////////////////////
//		更新
/////////////////////////////////////////////////////////////////////////////////////////
void BaseBullet::Update() {
    float deltaTime = ClockManager::GetInstance()->GetDeltaTime();

    if (!isExploding_) {
        // 通常移動とtrailFx_更新
        worldTransform_.translation += velocity_ * moveSpeed_ * deltaTime;

        BaseGameObject::Update();

        if (trailFx_) {
            trailFx_->position_ = GetWorldPosition();
            trailFx_->Update();  // 明示的にUpdate呼ぶ（Attachだけで自動呼びでなければ）
        }

        // 寿命減少
        lifeTime_ -= deltaTime;

        if (lifeTime_ <= 0.0f) {
            // 寿命切れ → 爆発開始
            isExploding_ = true;

            // trailFx_停止＆破棄
            if (trailFx_) {
                FxIntermediary::GetInstance()->Detach(trailFx_.get());
                trailFx_.reset();
            }

            // 爆発エフェクト初期化・再生
            if (!explosionFx_) {
                explosionFx_ = std::make_unique<FxEmitter>();
                explosionFx_->LoadConfig("Resources/Assets/Configs/Effect/Explosion.json");
                FxIntermediary::GetInstance()->Attach(explosionFx_.get());
            }
            explosionFx_->position_ = GetWorldPosition();
            explosionFx_->Play();
        }
    } else {
        // 爆発中は爆発エフェクトの更新のみ
        if (explosionFx_) {
            explosionFx_->position_ = GetWorldPosition();
            explosionFx_->Update();

            // エフェクトが終了していたら弾も消す
            if (!explosionFx_->isPlayng() && explosionFx_->GetUnits().empty()) {
                isAlive_ = false;

                FxIntermediary::GetInstance()->Detach(explosionFx_.get());
                explosionFx_.reset();
            }
        }
    }
}

/////////////////////////////////////////////////////////////////////////////////////////
//		imgui
/////////////////////////////////////////////////////////////////////////////////////////
void BaseBullet::DerivativeGui(){

}

void BaseBullet::OnCollisionEnter([[maybe_unused]] Collider* other) {
}

void BaseBullet::OnShot() {
}