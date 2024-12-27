#include "DebugCamera.h"

// lib
#include "lib/myFunc/MyFunc.h"
#include "Engine/core/Input.h"

// Engine
#include "Engine/graphics/camera/CameraManager.h"

// C++
#include <algorithm>  // std::clamp
#undef max
#include <numbers>    // std::numbers::pi

DebugCamera::DebugCamera()
	: BaseCamera(){
	SetName("DebugCamera", ObjectType::Camera);
}

//////////////////////////////////////////////////////////////////////////
//							メイン処理
//////////////////////////////////////////////////////////////////////////
void DebugCamera::Update(){
	// 入力に基づいてカメラ操作
	Rotate();
	Move();
	Zoom();

	// カメラの姿勢・行列を更新
	{
		// orbitAngle_.x = 水平方向(Yaw)
		// orbitAngle_.y = 垂直方向(Pitch)
		// 距離と角度からカメラの相対座標を計算
		Matrix4x4 matRotYaw = MakeRotateYMatrix(orbitAngle_.x);
		Matrix4x4 matRotPitch = MakeRotateXMatrix(orbitAngle_.y);
		Matrix4x4 matRot = Matrix4x4::Multiply(matRotPitch, matRotYaw);

		// Z方向に距離分だけオフセットし、回転行列を適用
		Vector3 offset(0.0f, 0.0f, -distance_);
		offset = TransformNormal(offset, matRot);

		// カメラの位置 = ターゲット + オフセット
		transform_.translate = target_ + offset;

		// カメラの回転は (Pitch, Yaw, 0)
		transform_.rotate = Vector3(orbitAngle_.y, orbitAngle_.x, 0.0f);
	}

	// BaseCameraの更新処理を呼び出す
	BaseCamera::Update();
}

//////////////////////////////////////////////////////////////////////////
//							ヘルパー関数
//////////////////////////////////////////////////////////////////////////

//*-----------------------------------------------------------------------
// 回転処理 (MMBドラッグ): ターゲット中心にカメラを回転させる
//-----------------------------------------------------------------------
void DebugCamera::Rotate(){
	if (Input::PushMouseButton(2)){
		bool shiftHeld = Input::PushKey(DIK_LSHIFT) || Input::PushKey(DIK_RSHIFT);
		bool ctrlHeld = Input::PushKey(DIK_LCONTROL) || Input::PushKey(DIK_RCONTROL);

		if (!shiftHeld && !ctrlHeld){
			Vector2 mouseDelta = Input::GetMouseDelta(); // 各フレームの移動量を取得

			// マウスの移動量に基づいて回転
			orbitAngle_.x -= mouseDelta.x * rotateSpeed_;
			orbitAngle_.y -= mouseDelta.y * rotateSpeed_;

			// ピッチ角度を制限 (上下90度未満)
			float limit = static_cast< float >(std::numbers::pi) * 0.5f - 0.01f;
			orbitAngle_.y = std::clamp(orbitAngle_.y, -limit, limit);
		}
	}
}


//*-----------------------------------------------------------------------
// パン処理 (Shift + MMBドラッグ): カメラのターゲットを移動させる
//-----------------------------------------------------------------------
void DebugCamera::Move(){
	if (Input::PushMouseButton(2)){
		bool shiftHeld = Input::PushKey(DIK_LSHIFT) || Input::PushKey(DIK_RSHIFT);
		bool ctrlHeld = Input::PushKey(DIK_LCONTROL) || Input::PushKey(DIK_RCONTROL);

		if (shiftHeld && !ctrlHeld){
			Vector2 mouseDelta = Input::GetMouseDelta(); // 各フレームの移動量を取得

			// カメラの回転行列を作成
			Matrix4x4 matRotYaw = MakeRotateYMatrix(orbitAngle_.x);
			Matrix4x4 matRotPitch = MakeRotateXMatrix(orbitAngle_.y);
			Matrix4x4 matRot = Matrix4x4::Multiply(matRotPitch, matRotYaw);

			// パン方向の移動量 (画面右が-X, 上が+Yになるよう調整)
			Vector3 localMove(
				-mouseDelta.x * panSpeed_,
				mouseDelta.y * panSpeed_,
				0.0f
			);

			// ローカル移動量をワールド座標に変換
			Vector3 worldMove = TransformNormal(localMove, matRot);

			// ターゲット位置を移動
			target_ += worldMove;
		}
	}
}
//*-----------------------------------------------------------------------
// ズーム処理 (Ctrl + MMBドラッグ or マウスホイール): カメラの距離を変更
//-----------------------------------------------------------------------
void DebugCamera::Zoom(){
	// ホイールクリックが押されている場合はズームを無視
	if (Input::PushMouseButton(2)){
		return;
	}

	// マウスホイールによるズーム処理
	float wheel = Input::GetMouseWheel(); // 1フレーム当たりのホイール回転量
	if (wheel != 0.0f){
		distance_ -= wheel * (zoomSpeed_ * 5.0f);
		distance_ = std::max(0.01f, distance_);
	}
}
