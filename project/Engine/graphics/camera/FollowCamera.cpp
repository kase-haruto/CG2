#include "FollowCamera.h"

// lib
#include "lib/myFunc/MyFunc.h"

#include "Engine/core/Input.h"
#include "Engine/graphics/camera/CameraManager.h"

//c++
#include <numbers>

void FollowCamera::Initialize(){}

void FollowCamera::Update(){

	//* 追従
	Adulation();

	//* 旋回
	Turning();

	//* 行列の更新
	UpdateMatrix();

}

Vector3 FollowCamera::CalculateOffset(){
	Vector3 result = offset;
	//カメラの角度から回転行列を計算
	Matrix4x4 matRotateX = MakeRotateXMatrix(rotation_.x);
	Matrix4x4 matRotateY = MakeRotateYMatrix(rotation_.y);
	Matrix4x4 matRotateZ = MakeRotateZMatrix(rotation_.z);
	Matrix4x4 matRotate = Matrix4x4::Multiply(Matrix4x4::Multiply(matRotateX, matRotateY), matRotateZ);
	result = TransformNormal(result, matRotate);
	return result;
}

void FollowCamera::Turning(){
	if (!target_){
		return; // ターゲットが存在しない場合は処理しない
	}

	float deltaX = Input::GetRightStick().x;

	destinationAngle_.y += deltaX * rotateSpeed*deltaTime;

	// 垂直方向の回転角度の制限（必要に応じて）
	const float maxVerticalAngle = float(std::numbers::pi) / 2.0f - 0.1f; // 制限値を調整
	destinationAngle_.x = std::clamp(destinationAngle_.x, -maxVerticalAngle, maxVerticalAngle);

	// 水平方向および垂直方向の回転をスムーズに補間
	rotation_.y = LerpShortAngle(rotation_.y, destinationAngle_.y, 0.1f);

}

void FollowCamera::Adulation(){

	if (target_){
		//追従座標の補完
		interTarget_ = Vector3::Lerp(interTarget_, target_->translate, 0.1f);

		Vector3 cameraOffset = CalculateOffset();
		//座標をコピーしてオフセット分ずらす
		translation_ = interTarget_ + cameraOffset;
	}

}


void FollowCamera::UpdateMatrix(){
	// 回転行列の作成
	rotateMatrix_ = EulerToMatrix(rotation_);

	// ワールド行列の初期化
	worldMat_ = Matrix4x4::MakeIdentity();

	// 平行移動行列の作成
	Matrix4x4 translateMatrix = MakeTranslateMatrix(translation_);

	// 回転と平行移動を適用
	worldMat_ = Matrix4x4::Multiply(rotateMatrix_, translateMatrix);

	// ビュー行列の計算（カメラのワールド行列の逆行列）
	Matrix4x4 viewMatrix = Matrix4x4::Inverse(worldMat_);

	// ビュー行列とプロジェクション行列の掛け算
	viewProjectionMatrix_ = Matrix4x4::Multiply(viewMatrix, CameraManager::GetInstance()->GetCamera3d()->GetProjectionMatrix());
}