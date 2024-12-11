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
	BoxCollider::colliderType_ = ColliderType::Type_Player;
	BoxCollider::targetColliderType_ = ColliderType::Type_Enemy;

	CollisionManager::GetInstance()->AddCollider(this);

	weapon_ = std::make_unique<Weapon>("weapon");
	horizontalSlash_ = std::make_unique< HorizontalSlash>();
	horizontalSlash_->SetPlayer(this);

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

	weapon_->Initialize();

}

void Player::Update(){

	//移動
	Move();

	//weaponをplayerの子に設定
	
	weapon_->UpdateWorldMat();
	Matrix4x4 worldMat = Matrix4x4::Multiply(model_->worldMatrix, weapon_->GetWorldMat());
	weapon_->SetWorldMat(worldMat);

	weapon_->Update();

	// HorizontalSlash の実行
	if (isAttacking_){
		horizontalSlash_->Execution();

		if (!horizontalSlash_->GetIsAttacking()){
			isAttacking_ = false; // 攻撃が終了したらフラグをリセット
		}
	} else{
		// スペースキーで攻撃を開始
		if (Input::TriggerGamepadButton(XINPUT_GAMEPAD_X)){
			isAttacking_ = true;
			horizontalSlash_->Initialize();
		}
	}

	shape_.center = model_->transform.translate;
	shape_.rotate = model_->transform.rotate;
	shape_.size = model_->transform.scale + 0.1f;

	BaseGameObject::Update();

}

void Player::Draw(){

	BoxCollider::Draw();

	horizontalSlash_->Draw();
	
	//weapon_->Draw();
	
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
void Player::OnCollisionEnter([[maybe_unused]] Collider* other){

	//* 衝突相手がtargetType_に含まれていなければreturnsa
	if ((other->GetType() & Collider::GetTargetType()) != ColliderType::Type_None){


	}

}

void Player::OnCollisionStay([[maybe_unused]] Collider* other){}

void Player::OnCollisionExit([[maybe_unused]] Collider* other){}


////////////////////////////////////////////////////////////////////////////
//						getter
////////////////////////////////////////////////////////////////////////////
Vector3 Player::GetForward(float distance) const{
	// ワールド行列の Z 軸方向ベクトルを取得
	Vector3 forward = Vector3(
		model_->worldMatrix.m[0][2], // 行列の第3列のX成分
		model_->worldMatrix.m[1][2], // 行列の第3列のY成分
		model_->worldMatrix.m[2][2]  // 行列の第3列のZ成分
	);

	// 単位ベクトル化して距離をスケーリング
	forward = forward.Normalize() * distance;

	return forward;
}

