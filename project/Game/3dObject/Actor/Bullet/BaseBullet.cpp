#include "BaseBullet.h"

/* ========================================================================
/* include space
/* ===================================================================== */

/* engine */
#include <Engine/Foundation/Clock/ClockManager.h>

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
}

/////////////////////////////////////////////////////////////////////////////////////////
//		更新
/////////////////////////////////////////////////////////////////////////////////////////
void BaseBullet::Update(){
	float deltaTime = ClockManager::GetInstance()->GetDeltaTime();
	worldTransform_.translation += velocity_ * moveSpeed_ * deltaTime;

	BaseGameObject::Update();

	// 時間カウント
	//currentTime_ += deltaTime;
	//if (currentTime_ >= lifeTime_){
	//	isAlive_ = false;
	//}
}

/////////////////////////////////////////////////////////////////////////////////////////
//		imgui
/////////////////////////////////////////////////////////////////////////////////////////
void BaseBullet::DerivativeGui(){

}
