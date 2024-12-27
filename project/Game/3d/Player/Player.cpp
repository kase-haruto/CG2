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
	//オブジェクトの名前を設定
	BaseGameObject::SetName("Player");
	Collider::SetName("Player");
}

Player::~Player(){

	CollisionManager::GetInstance()->RemoveCollider(this);

}

//===================================================================*/
//                    main functions
//===================================================================*/
void Player::Initialize(){
	

	BaseGameObject::Initialize();

	//colliderの設定
	BoxCollider::Initialize(model_->transform.scale);
	Collider::type_ = ColliderType::Type_Player;
	Collider::targetType_ = ColliderType::Type_Enemy;

	model_->transform.translate.z = -15.0f;
	moveSpeed_ = 10.0f;

	attackController_.SetPlayer(this);

}

void Player::Update(){
	Move();

	shape_.center = GetCenterPos();
	shape_.rotate = model_->transform.rotate;

	// 攻撃管理クラスの更新
	attackController_.Update();

	BaseGameObject::Update();
	Character::Update();
}

void Player::Draw(){

	BoxCollider::Draw();
	BaseGameObject::Draw();

	// 攻撃管理クラスの描画
	attackController_.Draw();

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
void Player::OnCollisionEnter([[maybe_unused]]Collider* other){
	
	//* 衝突相手がtargetType_に含まれていなければreturn
	if ((other->GetType() & Collider::GetTargetType()) != ColliderType::Type_None){


		//////////////////////////////////////////////////////////////////
		//				敵と衝突
		//////////////////////////////////////////////////////////////////
		if (other->GetType() == ColliderType::Type_Enemy){



		}

	}

}

void Player::OnCollisionStay([[maybe_unused]] Collider* other){}

void Player::OnCollisionExit([[maybe_unused]] Collider* other){}

//===================================================================*/
//                    imgui/ui
//===================================================================*/
void Player::ShowGui(){

	SceneObject::ShowGui();

	if (ImGui::Button("save")){
		JsonCoordinator::SaveGroup(BaseGameObject::GetName(),BaseGameObject::jsonPath);
	}

	ImGui::Separator();

	BaseGameObject::ShowGui();

	ImGui::Separator();

	BoxCollider::ShowGui();

	ImGui::Separator();

	attackController_.ShowGui();
}

//===================================================================*/
//                    getter
//===================================================================*/
const Vector3 Player::GetCenterPos()const {
	const Vector3 offset = {0.0f, 1.0f, 0.0f};
	Vector3 worldPos = Vector3::Transform(offset, model_->worldMatrix);
	return worldPos;
}