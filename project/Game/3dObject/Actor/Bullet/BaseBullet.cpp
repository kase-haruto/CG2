#include "BaseBullet.h"
/* ========================================================================
/* include space
/* ===================================================================== */

/* engine */
#include <Engine/Foundation/Clock/ClockManager.h>
#include <Engine/Objects/Collider/BoxCollider.h>

/* external */
#include <externals/imgui/imgui.h>

BaseBullet::BaseBullet(const std::string& modelName, const std::string& name)
	:Actor::Actor(modelName, name){
	collider_->SetType(ColliderType::Type_PlayerAttack);
	collider_->SetTargetType(ColliderType::Type_Enemy);
	collider_->SetOwner(this);
	collider_->SetIsDrawCollider(false);
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
	worldTransform_.translation += velocity_ * moveSpeed_ * deltaTime;

	BaseGameObject::Update();


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