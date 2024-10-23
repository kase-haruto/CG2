#include "Rail/RailCamera.h"
#include <algorithm>
#include <cmath>
#include "myFunc/MyFunc.h"

void RailCamera::Initialize(){
	transform_.scale = {1.0f,1.0f,1.0f};
	transform_.rotate = {0.0f,0.0f,0.0f};
	transform_.translate = {0.0f,2.0f,0.0f};
}

void RailCamera::Update(){
	t_ += 0.0004f;
	t_ = std::clamp(t_, 0.0f, 1.0f);

	// スプライン上の現在のカメラ位置を計算
	Vector3 eye = CatmullRomPosition(ctrlPoints_, t_);

	float t_2 = t_ + 0.03f;
	t_2 = std::clamp(t_2, 0.0f, 1.0f);

	// 注視点を求める
	Vector3 target = CatmullRomPosition(ctrlPoints_, t_2);

	// targetとeyeの差分ベクトル
	Vector3 forward = target - eye;

	// 方向ベクトルを正規化
	forward = forward.Normalize();

	// トランスフォームの更新
	transform_.translate = {eye.x,eye.y+2.0f,eye.z};
	float horizontalDistance = sqrtf(forward.x * forward.x + forward.z * forward.z);
	transform_.rotate.x = std::atan2(-forward.y, horizontalDistance);
	transform_.rotate.y = std::atan2(forward.x, forward.z);

	//worldMat_ = MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate);
}
