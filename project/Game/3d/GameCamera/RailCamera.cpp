#include "RailCamera.h"

#include <Engine/Foundation/Utility/Func/MyFunc.h>
#include <Engine/Foundation/Clock/ClockManager.h>
#include <Engine/Foundation/Utility/Func/MathFunc.h>
#include <Engine/Application/Input/Input.h>

// C++
#include <cmath>
#include <algorithm>

RailCamera::RailCamera() {}

void RailCamera::Initialize() {
	transform_.translate = { 0.0f, 10.0f, 0.0f };
	transform_.scale = { 1.0f, 1.0f, 1.0f };
	transform_.rotate = { 0.5f, 0.0f, 0.0f };

	worldTransform_.Initialize();
	worldTransform_.scale = transform_.scale;
	worldTransform_.eulerRotation = transform_.rotate;
	worldTransform_.translation = transform_.translate;

	BaseCamera::SetName("RailCamera");

	// 初期レール
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

void RailCamera::Update() {
	Input* input = Input::GetInstance();

	// カメラ傾き入力処理
	if (input->PushKey(DIK_D)) {
		targetTilt_ = -tiltAngle_;
	} else if (input->PushKey(DIK_A)) {
		targetTilt_ = tiltAngle_;
	} else {
		targetTilt_ = 0.0f;
	}

	float deltaTime = ClockManager::GetInstance()->GetDeltaTime();
	zTiltOffset_ = std::lerp(zTiltOffset_, targetTilt_, tiltLerpSpeed_ * deltaTime);

	// t を無限に進める（ループしない）
	float speed = 1.0f; // セグメント1つを1秒で進む
	t_ += speed * deltaTime;

	// 必要に応じてレールを延長
	int requiredPoints = static_cast<int>(t_) + 4;
	while ((int)railPoints_.size() < requiredPoints) {
		Vector3 last = railPoints_.back();
		railPoints_.push_back(last + Vector3(0, 0, 50)); // Z方向に直進
	}

	// 現在位置と注視点
	Vector3 eye = CatmullRomPosition(railPoints_, t_);
	Vector3 target = CatmullRomPosition(railPoints_, t_ + 0.01f);

	// forward ベクトルと回転
	Vector3 forward = target - eye;
	if (forward.LengthSquared() < 1e-6f) {
		forward = { 0.0f, 0.0f, 1.0f };
	}
	Vector3 normalizedForward = forward.Normalize();

	// トランスフォーム更新
	transform_.translate = eye;

	float horizontalDistance = sqrtf(normalizedForward.x * normalizedForward.x + normalizedForward.z * normalizedForward.z);
	transform_.rotate.x = std::atan2(-normalizedForward.y, horizontalDistance);
	transform_.rotate.y = std::atan2(normalizedForward.x, normalizedForward.z);
	transform_.rotate.z = zTiltOffset_;

	originalPosition_ = transform_.translate;

	worldTransform_.scale = transform_.scale;
	worldTransform_.eulerRotation = transform_.rotate;
	worldTransform_.translation = transform_.translate;

	BaseCamera::Update();
	worldTransform_.Update(viewProjectionMatrix_);
}

Vector3 RailCamera::CatmullRomPosition(const std::vector<Vector3>& points, float t) {
	int i = static_cast<int>(t);
	float localT = t - static_cast<float>(i);

	int maxIndex = static_cast<int>(points.size()) - 4;
	i = std::clamp(i, 0, maxIndex);

	const Vector3& p0 = points[i];
	const Vector3& p1 = points[i + 1];
	const Vector3& p2 = points[i + 2];
	const Vector3& p3 = points[i + 3];

	return CatmullRomInterpolation(p0, p1, p2, p3, localT);
}

void RailCamera::DrawRail() {
	// レール描画をしたい場合に使用（未実装）
}

Vector3 RailCamera::GetPosition() {
	Vector3 worldPos{};
	worldPos.x = worldMatrix_.m[3][0];
	worldPos.y = worldMatrix_.m[3][1];
	worldPos.z = worldMatrix_.m[3][2];
	return worldPos;
}
