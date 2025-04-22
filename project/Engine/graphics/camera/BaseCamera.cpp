// BaseCamera.cpp
#include "Engine/graphics/camera/BaseCamera.h"
#include "Engine/graphics/GraphicsGroup.h"
#include "lib/myFunc/MyFunc.h"
#include "Engine/core/Clock/ClockManager.h"
#include <cmath>
#include "BaseCamera.h"

BaseCamera::BaseCamera()
	:viewMatrix_(Matrix4x4::Inverse(worldMatrix_)),
	projectionMatrix_(MakePerspectiveFovMatrix(fovAngleY_, aspectRatio_, nearZ_, farZ_)){
	viewProjectionMatrix_ = Matrix4x4::Multiply(viewMatrix_, projectionMatrix_);

	/* バッファの生成とマッピング =======================*/
	CreateBuffer();

}

/////////////////////////////////////////////////////////////////////////
//  更新
/////////////////////////////////////////////////////////////////////////
void BaseCamera::Update(){
	if (!isActive_){ return; }//アクティブでない場合は処理しない

	// シェイク処理
	if (isShaking_){
		shakeElapsed_ += ClockManager::GetInstance()->GetDeltaTime();  // シングルトンから時間取得
		if (shakeElapsed_ < shakeDuration_){
			// ランダムなオフセットを生成（例：-1〜1の範囲で乱数を取得）
			float offsetX = ((rand() / ( float ) RAND_MAX) * 2.0f - 1.0f) * shakeIntensity_;
			float offsetY = ((rand() / ( float ) RAND_MAX) * 2.0f - 1.0f) * shakeIntensity_;
			float offsetZ = ((rand() / ( float ) RAND_MAX) * 2.0f - 1.0f) * shakeIntensity_;

			// 現在のカメラ位置にオフセットを加算
			transform_.translate = originalPosition_ + Vector3(offsetX, offsetY, offsetZ);
		} else{
			// シェイク終了時に元の位置に戻す
			isShaking_ = false;
			transform_.translate = originalPosition_;
		}
	}

	cameraBuffer_.TransferData(cameraData_);
	UpdateMatrix();

}

/////////////////////////////////////////////////////////////////////////
//  行列の更新
/////////////////////////////////////////////////////////////////////////
void BaseCamera::UpdateMatrix(){
	// 行列の更新
	worldMatrix_ = MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate);
	viewMatrix_ = Matrix4x4::Inverse(worldMatrix_);
	projectionMatrix_ = MakePerspectiveFovMatrix(fovAngleY_, aspectRatio_, nearZ_, farZ_);
	viewProjectionMatrix_ = Matrix4x4::Multiply(viewMatrix_, projectionMatrix_);
}

/////////////////////////////////////////////////////////////////////////
//  projection行列の作成
/////////////////////////////////////////////////////////////////////////
Matrix4x4 BaseCamera::MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip){
	Matrix4x4 result = {
		1 / (aspectRatio * std::tan(fovY / 2)), 0, 0, 0,
		0, 1 / std::tan(fovY / 2), 0, 0,
		0, 0, farClip / (farClip - nearClip), 1,
		0, 0, -nearClip * farClip / (farClip - nearClip), 0
	};
	return result;
}

/////////////////////////////////////////////////////////////////////////
//  カメラシェイク開始
/////////////////////////////////////////////////////////////////////////
void BaseCamera::StartShake(float duration, float intensity){
	if (!isShaking_){
		originalPosition_ = transform_.translate;  // 現在の位置を記憶
	}
	isShaking_ = true;
	shakeDuration_ = duration;
	shakeElapsed_ = 0.0f;
	shakeIntensity_ = intensity;
}

/////////////////////////////////////////////////////////////////////////
//  アクセッサ
/////////////////////////////////////////////////////////////////////////
#pragma region アクセッサ
void BaseCamera::SetName(const std::string& name){
	SceneObject::SetName(name, ObjectType::Camera);
}

void BaseCamera::SetCamera(const Vector3& pos, const Vector3& rotate){
	transform_.translate = pos;
	transform_.rotate = rotate;
}

const Matrix4x4& BaseCamera::GetViewMatrix() const{
	return viewMatrix_;
}

const Matrix4x4& BaseCamera::GetProjectionMatrix() const{
	return projectionMatrix_;
}

const Matrix4x4& BaseCamera::GetViewProjectionMatrix() const{
	return viewProjectionMatrix_;
}

const Vector3& BaseCamera::GetRotate() const{
	return transform_.rotate;
}

const Vector3& BaseCamera::GetTranslate() const{
	return transform_.translate;
}
#pragma endregion

/////////////////////////////////////////////////////////////////////////
//  バッファの生成とマッピング
/////////////////////////////////////////////////////////////////////////
#pragma region バッファの生成とマッピング
void BaseCamera::CreateBuffer(){
	ComPtr<ID3D12Device> device = GraphicsGroup::GetInstance()->GetDevice();
	cameraBuffer_.Initialize(device.Get());
	cameraBuffer_.TransferData(cameraData_);
}

void BaseCamera::Map(){
}
#pragma endregion


void BaseCamera::SetCommand(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> command, PipelineType pipelineType){
	ComPtr<ID3D12RootSignature> rootSignature = GraphicsGroup::GetInstance()->GetRootSignature(pipelineType);
	command->SetGraphicsRootSignature(rootSignature.Get());

	uint32_t rootParameterIndex = 0;
	if (pipelineType == PipelineType::Object3D||PipelineType::SkinningObject3D){
		rootParameterIndex = 5;
	}
	cameraBuffer_.SetCommand(command, rootParameterIndex);
}