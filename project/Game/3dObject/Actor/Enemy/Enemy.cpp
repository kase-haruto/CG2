#include "Enemy.h"

#include <Engine/Foundation/Utility/Random/Random.h>
#include <Engine/Foundation/Clock/ClockManager.h>
#include <Engine/Objects/Collider/BoxCollider.h>

/* ========================================================================
/* include space
/* ===================================================================== */
Enemy::Enemy(const std::string& modelName, const std::string objName) :
	Actor::Actor(modelName, objName) {

	worldTransform_.Initialize();
	worldTransform_.scale = { 2.0f, 2.0f, 2.0f };
	moveSpeed_ = Random::Generate<float>(1.0f, 3.0f);
	velocity_ = Random::GenerateVector3(-1.0f, 1.0f);

	collider_->SetType(ColliderType::Type_Enemy);
	collider_->SetTargetType(ColliderType::Type_Player);
	collider_->SetOwner(this);

	auto collider = dynamic_cast<BoxCollider*>(collider_.get());
	collider->SetSize(worldTransform_.scale+1);
	collider_->SetIsDrawCollider(false);

	life_ = 3;
}


/////////////////////////////////////////////////////////////////////////////////////////
//		初期化
/////////////////////////////////////////////////////////////////////////////////////////
void Enemy::Initialize() {}

/////////////////////////////////////////////////////////////////////////////////////////
//		更新
/////////////////////////////////////////////////////////////////////////////////////////
void Enemy::Update() {

	worldTransform_.Update();

	BaseGameObject::Update();

	if (life_<= 0) {
		isAlive_ = false;
	}
}

void Enemy::OnCollisionEnter([[maybe_unused]]Collider* other) {
	life_--;
}

const Vector3 Enemy::GetCenterPos() const {
	const Vector3 offset = { 0.0f, 0.0f, 0.0f };
	Vector3 worldPos = Vector3::Transform(offset, worldTransform_.matrix.world);
	return worldPos;
}

void Enemy::SetParent(WorldTransform* parent) {
	worldTransform_.parent = parent;
}

void Enemy::SetParent(SceneObject* newParent) {
	parent_ = newParent;
}

/////////////////////////////////////////////////////////////////////////////////////////
//		移動
/////////////////////////////////////////////////////////////////////////////////////////
void Enemy::Move() {}

/////////////////////////////////////////////////////////////////////////////////////////
//		弾発射
/////////////////////////////////////////////////////////////////////////////////////////
void Enemy::Shoot() {}
