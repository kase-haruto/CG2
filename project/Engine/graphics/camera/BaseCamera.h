// BaseCamera.h
#pragma once

#include "ICamera.h"
#include "Engine/objects/Transform.h"
#include "Engine/core/DirectX/Buffer/DxConstantBuffer.h"
#include "lib/myMath/Matrix4x4.h"
#include "lib/myMath/Vector3.h"

/* lib */
#include <numbers>

#include <d3d12.h>
#include <wrl.h>

using namespace Microsoft::WRL;

// 定数バッファ用データ構造体
struct Camera3dForGPU{
	Vector3 worldPosition;
	float padding;
};

class BaseCamera :
	public ICamera{
public:
	//==================================================================*//
	//			public functions
	//==================================================================*//
	BaseCamera();
	virtual ~BaseCamera() = default;

	virtual void Update();  // 更新

	virtual void UpdateMatrix();  // 行列の更新

	void SetCommand(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> command, PipelineType pipelineType);

	void StartShake(float duration, float intensity)override;  // カメラシェイク開始

private:
	//==================================================================*//
	//			private functions
	//==================================================================*//
	void CreateBuffer();
	void Map();


protected:
	//==================================================================*//
	//			protected functions
	//==================================================================*//
	Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip);
	void SetName(const std::string& name);


public:
	//==================================================================*//
	//			getter / setter
	//==================================================================*//
	// Setter
	void SetCamera(const Vector3& pos, const Vector3& rotate);

	// Getter
	const Matrix4x4& GetViewMatrix() const;
	const Matrix4x4& GetProjectionMatrix() const;
	const Matrix4x4& GetViewProjectionMatrix() const;
	const Vector3& GetRotate() const;
	const Vector3& GetTranslate() const;

protected:
	//==================================================================*//
	//			protected variables
	//==================================================================*//
	
	Matrix4x4 viewMatrix_;          // ビュー行列
	Matrix4x4 projectionMatrix_;    // プロジェクション行列

	float aspectRatio_ = 16.0f / 9.0f;                           // アスペクト比
	float nearZ_ = 0.1f;                                         // 近クリップ面
	float farZ_ = 1000.0f;                                       // 遠クリップ面
	float fovAngleY_ = 45.0f * static_cast< float >(std::numbers::pi) / 180.0f;  // 垂直視野角

protected:
	// カメラシェイク関連
	bool isShaking_ = false;
	float shakeDuration_ = 0.0f;
	float shakeElapsed_ = 0.0f;
	float shakeIntensity_ = 0.0f;  // シェイクの強さ
	Vector3 originalPosition_;     // シェイク前の元のカメラ位置


private:
	//==================================================================*//
	//			private variables
	//==================================================================*//
	ComPtr<ID3D12Resource> constBuffer_;			//< 定数バッファ
	DxConstantBuffer<Camera3dForGPU> cameraBuffer_;	//< 定数バッファデータ
	Camera3dForGPU* cameraData_;					//< カメラのデータ
};
