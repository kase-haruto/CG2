#include "Player.h"

/* ========================================================================
/* include space
/* ===================================================================== */

// engine
#include <Engine/Application/Input/Input.h>
#include <Engine/Foundation/Clock/ClockManager.h>
#include <Engine/Graphics/Camera/Manager/CameraManager.h>
#include <Engine/Application/System/Enviroment.h>
#include <Engine/Foundation/Utility/Ease/Ease.h>
#include <Engine/Foundation/Utility/Random/Random.h>
#include <Engine/Application/Effects/Intermediary/FxIntermediary.h>
#include <Engine/Physics/Ray/Raycastor.h>
#include <Engine/Physics/Ray/RayDetail.h>

// externals
#include <externals/imgui/imgui.h>
#include <Engine/Foundation/Utility/Func/MyFunc.h>

// c++
#include <numbers>

Player::Player(const std::string& modelName,
			   std::optional<std::string> objectName)
	:Actor::Actor(modelName, objectName) {
	worldTransform_.translation = { 0.0f, 0.0f, 15.0f };

	collider_->SetTargetType(ColliderType::Type_Enemy);
	collider_->SetType(ColliderType::Type_Player);
	model_->SetIsDrawEnable(false);
}

/////////////////////////////////////////////////////////////////////////////////////////
//		初期化
/////////////////////////////////////////////////////////////////////////////////////////
void Player::Initialize() {
	moveSpeed_ = 15.0f;
	InitializeEffect();
}

/////////////////////////////////////////////////////////////////////////////////////////
//		更新
/////////////////////////////////////////////////////////////////////////////////////////
void Player::Update() {
	//移動
	Move();

	if (rollSet_.isRolling_) {
		rollSet_.rollTimer_ += ClockManager::GetInstance()->GetDeltaTime();
		float t = rollSet_.rollTimer_ / rollSet_.rollDuration_;
		t = std::clamp(t, 0.0f, 1.0f);

		if (t >= 1.0f) {
			rollSet_.isRolling_ = false;
		}

		// Z軸回転（2秒で1回転）
		float angleOffset = rollSet_.rollDirection_ * std::numbers::pi_v<float> *2.0f;
		worldTransform_.eulerRotation.z =
			Lerp(rollSet_.rollStartAngle_, rollSet_.rollStartAngle_ + angleOffset, t);

		// Z移動（前進→戻る）
		float zRatio = EaseForwardThenReturn(t); // 0→1→0
		float zOffset = rollSet_.rollOffset_.z * zRatio;
		worldTransform_.translation =
			rollSet_.rollStartPos_ + Vector3(0.0f, 0.0f, zOffset);
	}

	if (Input::GetInstance()->TriggerKey(DIK_LSHIFT)) {
		BarrelRoll();
	}

	shootInterval_ -= ClockManager::GetInstance()->GetDeltaTime();
	if (Input::GetInstance()->PushKey(DIK_SPACE) && shootInterval_ <= 0.0f) {
		Shoot();
		shootInterval_ = kMaxShootInterval_;
	}

	bulletContainer_->Update();
	BaseGameObject::Update();
}


/////////////////////////////////////////////////////////////////////////////////////////
//		imgui
/////////////////////////////////////////////////////////////////////////////////////////
void Player::DerivativeGui() {
	ImGui::DragFloat("moveSpeed", &moveSpeed_, 0.01f, 0.0f, 10.0f);

	if (ImGui::BeginTabBar("FxEmittersTabBar")) {

		if (trailFx_ && ImGui::BeginTabItem("Trail")) {
			ImGui::PushID(trailFx_.get());
			trailFx_->ShowGui();
			ImGui::PopID();
			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}
}


///////////////////////////////////////////////////////////////////////////////////
//		移動
///////////////////////////////////////////////////////////////////////////////////
void Player::Move() {
	Vector3 moveVector = { 0.0f, 0.0f, 0.0f };;
	//キーボード移動
	if (Input::GetInstance()->PushKey(DIK_A)) {
		moveVector.x -= 1.0f;
	} else if (Input::GetInstance()->PushKey(DIK_D)) {
		moveVector.x += 1.0f;
	}

	if (Input::GetInstance()->PushKey(DIK_W)) {
		moveVector.y += 1.0f;
	} else if (Input::GetInstance()->PushKey(DIK_S)) {
		moveVector.y -= 1.0f;
	}

	//移動ベクトルを正規化
	if (moveVector.Length() > 0.0f) {
		moveVector.Normalize();
	}


	//移動速度を掛ける
	moveVector *= moveSpeed_;

	//エフェクトの座標更新
	trailFx_->position_ = GetWorldPosition();

	//移動ベクトルを加算
	worldTransform_.translation += moveVector * ClockManager::GetInstance()->GetDeltaTime();

	if (rollSet_.isRolling_) return;
	UpdateTilt(moveVector);
}

void Player::Shoot() {
	// マウス座標をInputから取得（Inputにマウス位置取得関数がある前提）
	Vector2 mousePos = Input::GetInstance()->GetMousePosition();

	// カメラのViewとProjectionを取得（CameraManagerが単一のMainCameraを持つ前提）
	auto mainCam = CameraManager::GetInstance()->GetCamera3d();
	Matrix4x4 view = mainCam->GetViewMatrix();
	Matrix4x4 proj = mainCam->GetProjectionMatrix();

	// ウィンドウやビューポートサイズ取得（例としてWindowクラスを使用）
	Vector2 viewportSize = Vector2{ 1920,1080 };

	// マウス座標からレイを作成
	Ray ray = Raycastor::ConvertMouseToRay(mousePos, view, proj, viewportSize);

	// プレイヤーの位置を発射点に
	Vector3 wPos = GetWorldPosition();

	// レイの方向を発射方向に
	Vector3 dir = ray.direction;

	// 弾を生成（モデル名・発射位置・発射方向）
	bulletContainer_->AddBullet("debugCube.obj", wPos, dir);
}


void Player::UpdateTilt(const Vector3& moveVector) {
	// 停止時は角度を戻す
	if (moveVector.Length() <= 0.001f) {
		worldTransform_.eulerRotation.z *= 0.9f; // 緩やかに戻す
		return;
	}

	const float maxTilt = 0.3f; // 最大30度まで
	float normalizedX = moveVector.Normalize().x;
	float targetTilt = -normalizedX * maxTilt;

	// 滑らかに傾ける
	worldTransform_.eulerRotation.z =
		LerpShortAngle(worldTransform_.eulerRotation.z, targetTilt, 0.2f);
	lastMoveVector_ = moveVector; // 最後の移動ベクトルを保存
}

///////////////////////////////////////////////////////////////////////////////////
//		バレルロール
///////////////////////////////////////////////////////////////////////////////////
void Player::BarrelRoll() {
	if (rollSet_.isRolling_) return;

	// 現在の移動方向を取得（前回移動量などがあればそこから）
	Vector3 moveVec = lastMoveVector_; // または別途保持しておく（Move() 内でセット）

	// 左なら左回転、右なら右回転
	rollSet_.rollDirection_ = (moveVec.x < 0.0f) ? 1.0f : -1.0f;

	rollSet_.rollStartAngle_ = worldTransform_.eulerRotation.z;
	rollSet_.rollStartPos_ = worldTransform_.translation;

	// 前方へ飛ぶオフセット（例：Z方向へ +2.0f）
	rollSet_.rollOffset_ = Vector3(0.0f, 0.0f, 7.0f); // 最大前進距離
	rollSet_.rollDuration_ = 0.5f;

	rollSet_.isRolling_ = true;
	rollSet_.rollTimer_ = 0.0f;
	rollSet_.rollStartAngle_ = worldTransform_.eulerRotation.z;

}

float Player::EaseForwardThenReturn(float t) {
	if (t < 0.5f) {
		float x = t / 0.5f;
		return x * (2 - x); // EaseOutQuad
	} else {
		float x = (t - 0.5f) / 0.5f;
		return 1.0f - (x * x); // EaseInQuad (逆補間)
	}
}

void Player::InitializeEffect() {
	const std::string path = "Resources/Assets/Configs/Effect/";
	trailFx_ = std::make_unique<FxEmitter>();
	trailFx_->LoadConfig(path+"PlayerTrail.json");
	FxIntermediary::GetInstance()->Attach(trailFx_.get());
}

