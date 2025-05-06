#include "Player.h"

/* ========================================================================
/* Collision
/* ===================================================================== */
#include <Engine/core/Input.h>
#include "Engine/core/Clock/ClockManager.h"
#include <Game/Effect/ParticleEffect/ParticleEffectCollection.h>
#include <Engine/graphics/camera/CameraManager.h>
#include <Engine/core/Enviroment.h>
#include <Engine/core/Math/Ease.h>

//externals
#include <externals/imgui/imgui.h>
#include <lib/myFunc/MyFunc.h>

#include <numbers>

Player::Player(const std::string& modelName,
			   std::function<void(IMeshRenderable*)> registerCB)
	:Actor::Actor(modelName, "player", registerCB){
	bulletContainer_ = std::make_unique<BulletContainer>("playerBulletContainer", registerCB);
	SceneObject::EnableGuiList();
	model_->worldTransform_.translation = {0.0f, 0.0f, 25.0f};
}

/////////////////////////////////////////////////////////////////////////////////////////
//		初期化
/////////////////////////////////////////////////////////////////////////////////////////
void Player::Initialize(){
	moveSpeed_ = 10.0f;
}

/////////////////////////////////////////////////////////////////////////////////////////
//		更新
/////////////////////////////////////////////////////////////////////////////////////////
void Player::Update(){
	//移動
	Move();


	if (rollSet_.isRolling_){
		rollSet_.rollTimer_ += ClockManager::GetInstance()->GetDeltaTime();
		float t = rollSet_.rollTimer_ / rollSet_.rollDuration_;
		if (t >= 1.0f){
			t = 1.0f;
			rollSet_.isRolling_ = false;
		}

		float easedT = EvoEase::EaseInOutSine(t); // ← イージングをここで適用
		float angleOffset = rollSet_.rollDirection_ * std::numbers::pi_v<float> *2.0f;

		model_->worldTransform_.eulerRotation.z =
			Lerp(rollSet_.rollStartAngle_, rollSet_.rollStartAngle_ + angleOffset, easedT);
	}

	if (Input::GetInstance()->TriggerKey(DIK_LSHIFT)){
		BarrelRoll();
	}

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
	} else if (Input::GetInstance()->PushKey(DIK_D)){
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
	//effect
	Vector3 wPos = model_->GetWorldTransform().GetWorldPosition();
	Vector3 offset = {0.0f, 0.0f, -2.0f};

	//移動速度を掛ける
	moveVector *= moveSpeed_;

	//移動ベクトルを加算
	model_->worldTransform_.translation += moveVector * ClockManager::GetInstance()->GetDeltaTime();

	ParticleEffectCollection::GetInstance()->PlayByName("smoke", wPos + offset, EmitType::Auto);
	if (rollSet_.isRolling_) return;
	UpdateTilt(moveVector);
}

void Player::Shoot(){
	//弾を追加
	Vector3 wPos = model_->GetWorldTransform().GetWorldPosition();
	Vector3 offset = {0.0f, 0.7f, 2.0f};
	//発射方向

	//Vector2 mousePos = Input::GetInstance()->GetMousePosition();
	//Matrix4x4 viewProjMatrix = CameraManager::GetViewProjectionMatrix();
	//Matrix4x4 matviewPort = Matrix4x4::MakeViewportMatrix(0, 0, 1280,720, 0, 1);
	//Matrix4x4 matVPV = Matrix4x4::Multiply(viewProjMatrix, matviewPort);
	//Matrix4x4 matInvVPV = Matrix4x4::Inverse(matVPV);

	//Vector3 posNear = Vector3(mousePos.x, mousePos.y, 0.0f);
	//Vector3 posFar = Vector3(mousePos.x, mousePos.y, 1.0f);

	//posNear = Matrix4x4::Transform(posNear, matInvVPV);
	//posFar = Matrix4x4::Transform(posFar, matInvVPV);

	//Vector3 shootDir = (posFar - posNear).Normalize();
	Vector3 dir = Vector3 {0.0f,0.0f,1.0f};
	bulletContainer_->AddBullet("debugCube.obj", wPos, dir);
	ParticleEffectCollection::GetInstance()->PlayByName("shootEffect", wPos + offset);
}


void Player::UpdateTilt(const Vector3& moveVector){
	// 停止時は角度を戻す
	if (moveVector.Length() <= 0.001f){
		model_->worldTransform_.eulerRotation.z *= 0.9f; // 緩やかに戻す
		return;
	}

	const float maxTilt = 0.3f; // 最大30度まで
	float normalizedX = moveVector.Normalize().x;
	float targetTilt = -normalizedX * maxTilt;

	// 滑らかに傾ける
	model_->worldTransform_.eulerRotation.z =
		LerpShortAngle(model_->worldTransform_.eulerRotation.z, targetTilt, 0.2f);
	lastMoveVector_ = moveVector; // 最後の移動ベクトルを保存
}

///////////////////////////////////////////////////////////////////////////////////
//		バレルロール
///////////////////////////////////////////////////////////////////////////////////
void Player::BarrelRoll(){
	if (rollSet_.isRolling_) return;

	// 現在の移動方向を取得（前回移動量などがあればそこから）
	Vector3 moveVec = lastMoveVector_; // または別途保持しておく（Move() 内でセット）

	// 左なら左回転、右なら右回転
	rollSet_.rollDirection_ = (moveVec.x < 0.0f) ? 1.0f : -1.0f;

	rollSet_.isRolling_ = true;
	rollSet_.rollTimer_ = 0.0f;
	rollSet_.rollStartAngle_ = model_->worldTransform_.eulerRotation.z;

	Vector3 wPos = model_->GetWorldTransform().GetWorldPosition();
	ParticleEffectCollection::GetInstance()->PlayByName("reloadParticle", wPos, EmitType::Once);
}