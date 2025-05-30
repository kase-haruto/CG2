#include "Enemy.h"

#include <Engine/Foundation/Utility/Random/Random.h>
#include <Engine/Foundation/Clock/ClockManager.h>
#include <Game/Effect/ParticleEffect/ParticleEffectSystem.h>
#include <Engine/Objects/Collider/BoxCollider.h>

/* ========================================================================
/* include space
/* ===================================================================== */
Enemy::Enemy(const std::string& modelName, const std::string objName) :
	Actor::Actor(modelName, objName) {

	worldTransform_.scale = { 2.0f, 2.0f, 2.0f };
	moveSpeed_ = Random::Generate<float>(1.0f, 3.0f);
	velocity_ = Random::GenerateVector3(-1.0f, 1.0f);

	collider_->SetType(ColliderType::Type_Enemy);
	collider_->SetTargetType(ColliderType::Type_Player);
	collider_->SetOwner(this);

	auto collider = dynamic_cast<BoxCollider*>(collider_.get());
	collider->SetSize(worldTransform_.scale+1);


	InitializeEffect();

	life_ = 3;
}

void Enemy::InitializeEffect() {
	Vector3 wPos = worldTransform_.GetWorldPosition();
	hitEffect_ = ParticleEffectSystem::GetInstance()->CreateEffectByName("HitEffect", wPos, EmitType::Once);
}

/////////////////////////////////////////////////////////////////////////////////////////
//		初期化
/////////////////////////////////////////////////////////////////////////////////////////
void Enemy::Initialize() {}

/////////////////////////////////////////////////////////////////////////////////////////
//		更新
/////////////////////////////////////////////////////////////////////////////////////////
void Enemy::Update() {

	if (isHit_) {
		Vector3 wPos = worldTransform_.GetWorldPosition();
		Vector3 offset = { 0.0f, 0.0f, -1.0f };
		//ParticleEffectCollection::GetInstance()->PlayByName("shootEffect", wPos + offset);
		isHit_ = false;
	}

	BaseGameObject::Update();

	if (life_<= 0) {
		isAlive_ = false;
	}
}

void Enemy::OnCollisionEnter([[maybe_unused]]Collider* other) {
	if (hitEffect_) {
		Vector3 offset = { 0.0f, 0.0f, -2.0f };
		Vector3 hitPos = worldTransform_.GetWorldPosition()+ offset;
		hitEffect_->Play(hitPos,EmitType::Once);

		life_--;
	}
}

const Vector3 Enemy::GetCenterPos() const {
	const Vector3 offset = { 0.0f, 0.0f, 0.0f };
	Vector3 worldPos = Vector3::Transform(offset, worldTransform_.matrix.world);
	return worldPos;
}

/////////////////////////////////////////////////////////////////////////////////////////
//		移動
/////////////////////////////////////////////////////////////////////////////////////////
void Enemy::Move() {}

/////////////////////////////////////////////////////////////////////////////////////////
//		弾発射
/////////////////////////////////////////////////////////////////////////////////////////
void Enemy::Shoot() {}
