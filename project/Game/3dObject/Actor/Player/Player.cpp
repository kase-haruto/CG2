#include "Player.h"

/* ========================================================================
/* Collision
/* ===================================================================== */
#include <Engine/core/Input.h>
#include "Engine/core/Clock/ClockManager.h"
#include <Game/Effect/ParticleEffect/ParticleEffectCollection.h>
//externals
#include <externals/imgui/imgui.h>

Player::Player(const std::string& modelName,
			   std::function<void(IMeshRenderable*)> registerCB)
:Actor::Actor(modelName,"player",registerCB){
	bulletContainer_ = std::make_unique<BulletContainer>("playerBulletContainer",registerCB);
	SceneObject::EnableGuiList();

}

/////////////////////////////////////////////////////////////////////////////////////////
//		初期化
/////////////////////////////////////////////////////////////////////////////////////////
void Player::Initialize(){
	moveSpeed_ = 6.0f;
}

/////////////////////////////////////////////////////////////////////////////////////////
//		更新
/////////////////////////////////////////////////////////////////////////////////////////
void Player::Update(){
	//移動
	Move();

	shootInterval_ -= ClockManager::GetInstance()->GetDeltaTime();
	if (Input::GetInstance()->PushKey(DIK_SPACE) && shootInterval_ <= 0.0f){
		Shoot();
		shootInterval_ = kMaxShootInterval_;
	}

	bulletContainer_->Update();
	BaseGameObject::Update();
}


/////////////////////////////////////////////////////////////////////////////////////////
//		imgui
/////////////////////////////////////////////////////////////////////////////////////////
void Player::DerivativeGui(){
	ImGui::DragFloat("moveSpeed", &moveSpeed_, 0.01f, 0.0f, 10.0f);
}

///////////////////////////////////////////////////////////////////////////////////
//		移動
///////////////////////////////////////////////////////////////////////////////////
void Player::Move(){
	Vector3 moveVector = {0.0f, 0.0f, 0.0f};;
	//キーボード移動
	if (Input::GetInstance()->PushKey(DIK_A)){
		moveVector.x -= 1.0f;
	} else if(Input::GetInstance()->PushKey(DIK_D)){
		moveVector.x += 1.0f;
	}

	if (Input::GetInstance()->PushKey(DIK_W)){
		moveVector.y += 1.0f;
	} else if (Input::GetInstance()->PushKey(DIK_S)){
		moveVector.y -= 1.0f;
	}

	//移動ベクトルを正規化
	if (moveVector.Length() > 0.0f){
		moveVector.Normalize();
	}

	//移動速度を掛ける
	moveVector *= moveSpeed_;

	//移動ベクトルを加算
	model_->worldTransform_.translation += moveVector * ClockManager::GetInstance()->GetDeltaTime();
}

void Player::Shoot(){
	//弾を追加
	Vector3 wPos = model_->GetWorldTransform().GetWorldPosition();
	Vector3 offset = {0.0f, 0.0f, 0.5f};
	//発射方向
	Vector3 shootDir = Vector3::Forward();
	bulletContainer_->AddBullet("debugCube.obj", wPos, shootDir);
	ParticleEffectCollection::GetInstance()->PlayByName("shootEffect",wPos+offset);
}
