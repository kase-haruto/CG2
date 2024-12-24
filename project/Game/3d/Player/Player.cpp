#include "Player.h"

#include "Engine/collision/CollisionManager.h"
#include "lib/myFunc/MyFunc.h"

#include "Engine/graphics/camera/CameraManager.h"
#include "Engine/core/Json/JsonCoordinator.h"

#include <externals/imgui/imgui.h>
#include "Engine/core/Input.h"

Player::Player(const std::string& modelName)
	:Character(modelName){

	CollisionManager::GetInstance()->AddCollider(this);

}

Player::~Player(){

	CollisionManager::GetInstance()->RemoveCollider(this);

}

//===================================================================*/
//                    main functions
//===================================================================*/
void Player::Initialize(){

	BaseGameObject::SetName("Player");
	Collider::SetName("Player");

	BaseGameObject::Initialize();
	BoxCollider::Initialize(model_->transform.scale);

	model_->transform.translate.z = -15.0f;

	moveSpeed_ = 10.0f;

}

void Player::Update(){
	Move();

	shape_.center = GetCenterPos();
	shape_.rotate = model_->transform.rotate;

	BaseGameObject::Update();
	Character::Update();
}

void Player::Draw(){

	BoxCollider::Draw();
	BaseGameObject::Draw();

}

void Player::Move(){

	Vector3 moveDirection = {Input::GetLeftStick().x,0.0f,Input::GetLeftStick().y};
	moveVelocity_ = moveDirection * moveSpeed_;
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


//===================================================================*/
//                    collision
//===================================================================*/
void Player::OnCollisionEnter([[maybe_unused]]Collider* other){}

void Player::OnCollisionStay([[maybe_unused]] Collider* other){}

void Player::OnCollisionExit([[maybe_unused]] Collider* other){}

//===================================================================*/
//                    imgui/ui
//===================================================================*/
void Player::ShowGui(){
	std::string saveJsonPath = "gameobject/" + BaseGameObject::GetName();
	if (ImGui::Button("save")){
		JsonCoordinator::SaveGroup(BaseGameObject::GetName());
	}

	ImGui::Separator();

	BaseGameObject::ShowGui();

	ImGui::Separator();

	BoxCollider::ShowGui();
}

//===================================================================*/
//                    getter
//===================================================================*/
const Vector3 Player::GetCenterPos()const {
	const Vector3 offset = {0.0f, 1.0f, 0.0f};
	Vector3 worldPos = Vector3::Transform(offset, model_->worldMatrix);
	return worldPos;
}