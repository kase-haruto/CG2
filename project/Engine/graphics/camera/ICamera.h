#pragma once
#include "Engine/objects/SceneObject.h"
#include "lib/myMath/Matrix4x4.h"

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

protected:
	//==================================================================*//
	//			protected variables
	//==================================================================*//
	bool isActive_ = true;				//アクティブかどうか
	Matrix4x4 viewProjectionMatrix_;	// ビュープロジェクション行列



};

