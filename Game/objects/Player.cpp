#include "objects/Player.h"
#include "myFunc/MyFunc.h"
#include "myFunc/MathFunc.h"

void Player::Initialize(Model* model){
	Character::Initialize(model);
}

void Player::Update(){

	t_ += 0.0004f;
	t_ = std::clamp(t_, 0.0f, 1.0f);

	// スプライン上の現在のカメラ位置を計算
	Vector3 eye = CatmullRomPosition(ctrlPoints_, t_);

	float t_2 = t_ + 0.01f;
	t_2 = std::clamp(t_2, 0.0f, 1.0f);

	// 注視点を求める
	Vector3 target = CatmullRomPosition(ctrlPoints_, t_2);

	// targetとeyeの差分ベクトル
	Vector3 forward = target - eye;

	// 方向ベクトルを正規化
	forward = forward.Normalize();

	// トランスフォームの更新
	model_->transform.translate = eye;
	float horizontalDistance = sqrtf(forward.x * forward.x + forward.z * forward.z);
	model_->transform.rotate.x = std::atan2(-forward.y, horizontalDistance);
	model_->transform.rotate.y = std::atan2(forward.x, forward.z);

	//親がいたらそれも計算
	if (parentTransform_){
		parentWorldMat_ = MakeAffineMatrix(parentTransform_->scale,
										   parentTransform_->rotate,
										   parentTransform_->translate);
		model_->worldMatrix = Matrix4x4::Multiply(model_->worldMatrix, parentWorldMat_);
	}

	//行列の更新
	Character::Update();
}

void Player::Draw(){
	if (!isRail_){
		//Character::Draw();
	}
}
