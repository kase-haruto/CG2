#pragma once
#include "Engine/objects/SceneObject.h"
#include "lib/myMath/Matrix4x4.h"
#include "Engine/objects/Transform.h" 

class ICamera
	:public SceneObject{
public:
	//==================================================================*//
	//			public functions
	//==================================================================*//
	virtual~ICamera() = default;
	ICamera();

	virtual void Update() = 0;
	virtual void UpdateMatrix() = 0;
	virtual void ShowGui()override;

public:
	//==================================================================*//
	//			getter/setter
	//==================================================================*//
	bool IsActive()const{ return isActive_; }
	void SetActive(bool isActive){ isActive_ = isActive; }

	Matrix4x4 GetViewProjection()const{ return viewProjectionMatrix_; }
	const Matrix4x4& GetWorldMat() const;

protected:
	//==================================================================*//
	//			protected variables
	//==================================================================*//
	EulerTransform transform_ = {
		{1.0f, 1.0f, 1.0f},         // scale
		{0.0f, 0.0f, 0.0f},         // rotate
		{0.0f, 4.0f, -15.0f}        // translate
	};

	bool isActive_ = true;				//アクティブかどうか
	Matrix4x4 viewProjectionMatrix_;	// ビュープロジェクション行列
	Matrix4x4 worldMatrix_;         // ワールド行列



};

