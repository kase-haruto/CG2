#include "Player.h"

#include "Engine/collision/CollisionManager.h"
#include "Engine/graphics/camera/CameraManager.h"
// engine
#include "Engine/core/Input.h"

// lib
#include "lib/myFunc/MyFunc.h"

// externals
#include <externals/imgui/imgui.h>

Player::Player(const std::string& modelName)
	:BaseGameObject(modelName){

	BoxCollider::Initialize(model_->transform.scale);
	CollisionManager::GetInstance()->AddCollider(this);
}

Player::~Player(){

	CollisionManager::GetInstance()->RemoveCollider(this);

}



////////////////////////////////////////////////////////////////////////////
//						main Methods
////////////////////////////////////////////////////////////////////////////

void Player::Initialize(){

	BaseGameObject::SetName("player");
	BoxCollider::SetName("player");		//衝突ログ用
	BaseGameObject::Initialize();

}

void Player::Update(){

	//移動
	Move();

	shape_.center = model_->transform.translate;
	shape_.rotate = model_->transform.rotate;
	shape_.size = model_->transform.scale + 0.1f;

	BaseGameObject::Update();

}

void Player::Draw(){

	BoxCollider::Draw();
	BaseGameObject::Draw();

}

////////////////////////////////////////////////////////////////////////////
//						helper
////////////////////////////////////////////////////////////////////////////

void Player::Move(){

	Vector3 moveDirection = {Input::GetLeftStick().x,0.0f,Input::GetLeftStick().y};
	moveVelocity_ = moveDirection * speed_;
	Vector3 rotate = CameraManager::GetInstance()->GetFollowRotate();
	Matrix4x4 matRotateY = MakeRotateYMatrix(rotate.y);
	Matrix4x4 matRotateZ = MakeRotateZMatrix(rotate.z);
	Matrix4x4 matRotate = Matrix4x4::Multiply(matRotateY, matRotateZ);
	moveVelocity_ = Vector3::Transform(moveVelocity_, matRotate);

	model_->transform.translate += moveVelocity_ * deltaTime;

	float horizontalDistance = sqrtf(moveVelocity_.x * moveVelocity_.x + moveVelocity_.z * moveVelocity_.z);
	model_->transform.rotate.x = std::atan2(-moveVelocity_.y, horizontalDistance);

	// 目標角度を計算し、補間を適用
	targetAngle_ = std::atan2(moveVelocity_.x, moveVelocity_.z);
	model_->transform.rotate.y = LerpShortAngle(model_->transform.rotate.y, targetAngle_, 0.1f);

}

////////////////////////////////////////////////////////////////////////////
//						ui/imgui
////////////////////////////////////////////////////////////////////////////
void Player::ImGui(){

#ifdef _DEBUG
	BaseGameObject::ImGui();
#endif // _DEBUG


}

void Player::ShowDebugUI(){
	BaseGameObject::ShowDebugUI();
}


////////////////////////////////////////////////////////////////////////////
//						collision
////////////////////////////////////////////////////////////////////////////
void Player::OnCollisionEnter([[maybe_unused]] Collider* other){}

void Player::OnCollisionStay([[maybe_unused]] Collider* other){}

void Player::OnCollisionExit([[maybe_unused]] Collider* other){}

