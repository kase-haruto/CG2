#include "RailCamera.h"

#include <Engine/Foundation/Utility/Func/MyFunc.h>
#include <Engine/Foundation/Clock/ClockManager.h>
#include <Engine/Foundation/Utility/Func/MathFunc.h>

//c++
#include <cmath>
#include <algorithm>

RailCamera::RailCamera(){

}

/////////////////////////////////////////////////////////////////////////////////////////
//		初期化
/////////////////////////////////////////////////////////////////////////////////////////
void RailCamera::Initialize(){
	transform_.translate = {0.0f, 10.0f, 0.0f};
	transform_.scale = {1.0f, 1.0f, 1.0f};
	transform_.rotate = {0.5f, 0.0f, 0.0f};

	//ワールドトランスフォームに設定
	worldTransform_.Initialize();
	worldTransform_.scale = transform_.scale;
	worldTransform_.eulerRotation = transform_.rotate;
	worldTransform_.translation = transform_.translate;

	BaseCamera::SetName("RailCamera");

	railPoints_ = {
		{0,10,-100},
		{0,10,-50},
		{0,10,0},
		{0,10,50},
		{0,10,100},
		{0,10,150},
		{0,10,200},
		{0,10,250},
	};
}

/////////////////////////////////////////////////////////////////////////////////////////
//		更新
/////////////////////////////////////////////////////////////////////////////////////////
void RailCamera::Update(){

	// 時間経過に応じて t を更新（時間依存）
	float speed = 0.05f; // 1秒で20%進む
	t_ += speed * ClockManager::GetInstance()->GetDeltaTime();
	t_ = std::clamp(t_, 0.0f, 1.0f);

	// スプライン上の現在のカメラ位置を計算
	Vector3 eye = CatmullRomPosition(railPoints_, t_);

	// 注視点計算用の t を少し先に設定（終端で forward = 0 にならないよう）
	float t_2 = std::min(t_ + 0.001f, 0.999f);
	Vector3 target = CatmullRomPosition(railPoints_, t_2);

	// forward ベクトルの計算と正規化
	Vector3 forward = target - eye;
	if (forward.LengthSquared() < 1e-6f){
		forward = {0.0f, 0.0f, 1.0f}; // デフォルト方向
	}
	Vector3 normalizedForward = forward.Normalize();

	// トランスフォームの更新
	transform_.translate = eye;

	// 回転を算出
	float horizontalDistance = sqrtf(normalizedForward.x * normalizedForward.x + normalizedForward.z * normalizedForward.z);
	transform_.rotate.x = std::atan2(-normalizedForward.y, horizontalDistance);
	transform_.rotate.y = std::atan2(normalizedForward.x, normalizedForward.z);

	// Shake用に originalPosition_ を更新
	originalPosition_ = transform_.translate;

	worldTransform_.scale = transform_.scale;
	worldTransform_.eulerRotation = transform_.rotate;
	worldTransform_.translation = transform_.translate;
	// 共通カメラ処理呼び出し（Shakeや行列更新）
	BaseCamera::Update();
	worldTransform_.Update(viewProjectionMatrix_);
}


/////////////////////////////////////////////////////////////////////////////////////////
//		レールの描画
/////////////////////////////////////////////////////////////////////////////////////////
void RailCamera::DrawRail(){

}

Vector3 RailCamera::GetPosition() {
	Vector3 worldPos {};
	worldPos.x = worldMatrix_.m[3][0];
	worldPos.y = worldMatrix_.m[3][1];
	worldPos.z = worldMatrix_.m[3][2];

	return worldPos;
}
