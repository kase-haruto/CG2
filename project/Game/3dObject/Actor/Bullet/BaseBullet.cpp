#include "BaseBullet.h"
/* ========================================================================
/* include space
/* ===================================================================== */

/* engine */
#include <Engine/Foundation/Clock/ClockManager.h>
#include <Game/Effect/ParticleEffect/ParticleEffectSystem.h>

/* external */
#include <externals/imgui/imgui.h>

BaseBullet::BaseBullet(const std::string& modelName)
	:Actor::Actor(modelName, "bullet"){
	SphereCollider::name_ = "playerBullet";
	SphereCollider::Initialize(worldTransform_.scale.x * 2.0f + 0.1f);
	Collider::targetType_ = ColliderType::Type_PlayerAttack;
	Collider::type_ = ColliderType::Type_PlayerAttack;
}

/////////////////////////////////////////////////////////////////////////////////////////
//		初期化
/////////////////////////////////////////////////////////////////////////////////////////
void BaseBullet::Initialize(const Vector3 initPos, const Vector3 velocity){
	worldTransform_.translation = initPos;
	velocity_ = velocity;
	moveSpeed_ = 15.0f;
	life_ = 1;
	isAlive_ = true;

	OnShot();
}

/////////////////////////////////////////////////////////////////////////////////////////
//		更新
/////////////////////////////////////////////////////////////////////////////////////////
void BaseBullet::Update() {
	float deltaTime = ClockManager::GetInstance()->GetDeltaTime();
	worldTransform_.translation += velocity_ * moveSpeed_ * deltaTime;

	BaseGameObject::Update();

	if (bulletEffect_) {
		Vector3 wPos = worldTransform_.GetWorldPosition();
		bulletEffect_->Play(wPos, EmitType::Auto);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
//		imgui
/////////////////////////////////////////////////////////////////////////////////////////
void BaseBullet::DerivativeGui(){

}

void BaseBullet::OnShot() {
	Vector3 wPos = worldTransform_.GetWorldPosition();
	bulletEffect_ = ParticleEffectSystem::GetInstance()->CreateEffectByName("BulletEffect", wPos, EmitType::Both);
}