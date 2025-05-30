#include "Enemy.h"

#include <Engine/Foundation/Utility/Random/Random.h>
#include <Engine/Foundation/Clock/ClockManager.h>
#include <Game/Effect/ParticleEffect/ParticleEffectCollection.h>

/* ========================================================================
/* include space
/* ===================================================================== */
Enemy::Enemy(const std::string& modelName) :
	Actor::Actor(modelName, "enemy"){

	worldTransform_.scale = {5.0f, 5.0f, 5.0f};
	moveSpeed_ = Random::Generate<float>(1.0f, 3.0f);
	velocity_ = Random::GenerateVector3(-1.0f, 1.0f);

	collider_->SetType(ColliderType::Type_Enemy);
	collider_->SetTargetType(ColliderType::Type_Player);
}

/////////////////////////////////////////////////////////////////////////////////////////
//		初期化
/////////////////////////////////////////////////////////////////////////////////////////
void Enemy::Initialize(){
}

/////////////////////////////////////////////////////////////////////////////////////////
//		更新
/////////////////////////////////////////////////////////////////////////////////////////
void Enemy::Update(){

	if (isHit_){
		Vector3 wPos = worldTransform_.GetWorldPosition();
		Vector3 offset = {0.0f, 0.0f, -2.0f};
		//ParticleEffectCollection::GetInstance()->PlayByName("shootEffect", wPos + offset);
		isHit_ = false;
	}
	
	BaseGameObject::Update();
}

void Enemy::OnCollisionEnter(Collider* other){
	if (other->GetTargetType() == ColliderType::Type_PlayerAttack){
		
	}

	isHit_ = true;
}

/////////////////////////////////////////////////////////////////////////////////////////
//		移動
/////////////////////////////////////////////////////////////////////////////////////////
void Enemy::Move(){}

/////////////////////////////////////////////////////////////////////////////////////////
//		弾発射
/////////////////////////////////////////////////////////////////////////////////////////
void Enemy::Shoot(){}
