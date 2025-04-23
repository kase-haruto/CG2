#include "BaseBullet.h"

/* ========================================================================
/* include space
/* ===================================================================== */

/* engine */
#include <Engine/core/Clock/ClockManager.h>

/* external */
#include <externals/imgui/imgui.h>

BaseBullet::BaseBullet(const std::string& modelName,
					   std::function<void(IMeshRenderable*)> registerCB)
	:Actor::Actor(modelName, "bullet", registerCB){
}

/////////////////////////////////////////////////////////////////////////////////////////
//		初期化
/////////////////////////////////////////////////////////////////////////////////////////
void BaseBullet::Initialize(const Vector3 initPos, const Vector3 velocity){
	model_->worldTransform_.translation = initPos;
	velocity_ = velocity;
	moveSpeed_ = 15.0f;
	life_ = 1;
	isAlive_ = true;

	model_->UpdateMatrix();
}

/////////////////////////////////////////////////////////////////////////////////////////
//		更新
/////////////////////////////////////////////////////////////////////////////////////////
void BaseBullet::Update(){
	model_->worldTransform_.translation += velocity_ * moveSpeed_ * ClockManager::GetInstance()->GetDeltaTime();

	BaseGameObject::Update();

	if (life_ <= 0){
		isAlive_ = false;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
//		imgui
/////////////////////////////////////////////////////////////////////////////////////////
void BaseBullet::DerivativeGui(){

}
