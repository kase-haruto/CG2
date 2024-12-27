#include "FollowCamera.h"

// lib
#include "lib/myFunc/MyFunc.h"

#include "Engine/core/Input.h"
#include "Engine/graphics/camera/CameraManager.h"
#include <externals/imgui/imgui.h>
//c++
#include <numbers>

FollowCamera::FollowCamera()
:BaseCamera(){
	BaseCamera::SetName("FollowCamera");
}

void FollowCamera::Update(){

	//* 追従
	Adulation();

	//* 旋回
	Turning();

	BaseCamera::Update();
}

Vector3 FollowCamera::CalculateOffset(){
	Vector3 result = offset;
	//カメラの角度から回転行列を計算
	Matrix4x4 matRotateX = MakeRotateXMatrix(transform_.rotate.x);
	Matrix4x4 matRotateY = MakeRotateYMatrix(transform_.rotate.y);
	Matrix4x4 matRotateZ = MakeRotateZMatrix(transform_.rotate.z);
	Matrix4x4 matRotate = Matrix4x4::Multiply(Matrix4x4::Multiply(matRotateX, matRotateY), matRotateZ);
	result = TransformNormal(result, matRotate);
	return result;
}

void FollowCamera::Turning(){
	if (!target_){
		return; // ターゲットが存在しない場合は処理しない
	}

	float deltaX = Input::GetRightStick().x;

	destinationAngle_.y += deltaX * rotateSpeed * deltaTime;

	// 垂直方向の回転角度の制限（必要に応じて）
	const float maxVerticalAngle = float(std::numbers::pi) / 2.0f - 0.1f; // 制限値を調整
	destinationAngle_.x = std::clamp(destinationAngle_.x, -maxVerticalAngle, maxVerticalAngle);

	// 水平方向および垂直方向の回転をスムーズに補間
	transform_.rotate.y = LerpShortAngle(transform_.rotate.y, destinationAngle_.y, 0.1f);

}

void FollowCamera::Adulation(){

	if (target_){
		//追従座標の補完
		interTarget_ = Vector3::Lerp(interTarget_, target_->translate, 0.1f);

		Vector3 cameraOffset = CalculateOffset();
		//座標をコピーしてオフセット分ずらす
		transform_.translate = interTarget_ + cameraOffset;
	}

}

void FollowCamera::UpdateMatrix(){
	// 回転行列の作成
	rotateMatrix_ = EulerToMatrix(transform_.rotate);

	// ワールド行列の初期化
	worldMatrix_ = Matrix4x4::MakeIdentity();

	// 平行移動行列の作成
	Matrix4x4 translateMatrix = MakeTranslateMatrix(transform_.translate);

	// 回転と平行移動を適用
	worldMatrix_ = Matrix4x4::Multiply(rotateMatrix_, translateMatrix);

	// ビュー行列の計算（カメラのワールド行列の逆行列）
	Matrix4x4 viewMatrix = Matrix4x4::Inverse(worldMatrix_);

	projectionMatrix_ = MakePerspectiveFovMatrix(fovAngleY_, aspectRatio_, nearZ_, farZ_);

	// ビュー行列とプロジェクション行列の掛け算
	viewProjectionMatrix_ = Matrix4x4::Multiply(viewMatrix, projectionMatrix_);
}

void FollowCamera::ShowGui(){
	//名前の表示
	SceneObject::ShowGui();

	ImGui::Separator();

	// アクティブかどうか
	BaseCamera::ShowGui();
}