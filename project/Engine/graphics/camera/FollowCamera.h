#pragma once

#include "lib/myMath/Vector3.h"
#include "lib/myMath/Vector2.h"

#include "Engine/objects/Transform.h"

class FollowCamera{
public:
	//===================================================================*/
	//							public Methods
	//===================================================================*/

	FollowCamera() = default;
	~FollowCamera() = default;

	void Initialize();

	void Update();

private:
	//===================================================================*/
	//							private Methods
	//===================================================================*/

	
	Vector3 CalculateOffset();	//* オフセットの計算
	void Turning();				//* 旋回
	void Adulation();			//* 追従
	void UpdateMatrix();		//* 行列の更新


public:
	//===================================================================*/
	//					getter/setter
	//===================================================================*/

#pragma region アクセッサ
	//* 追従対象のセット
	void SetTarget(const Transform* target){ target_ = target; }

	void SetViewpoint(const Vector3& viewpoint){ viewpoint_ = viewpoint; }

	void SetPosition(const Vector3& pos){ translation_ = pos; }

	const Vector3& GetRotate(){ return rotation_; }

	const Vector3& GetTranslate()const{ return translation_; }

	const Matrix4x4& GetViewProjectionMat()const{ return viewProjectionMatrix_; }

	const Matrix4x4& GetWorldMat()const{ return worldMat_; }

#pragma endregion

private:
	//===================================================================*/
	//							private Methods
	//===================================================================*/

	const Transform* target_ = nullptr;			//* 追従対象
	Vector3 viewpoint_;							//* 注視点
	Vector3 interTarget_;						//* 追従対象の残像座標
	Vector3 offset = {0.0f,2.5f,-8.0f};			//* 追従対象からのオフセット
	float rotateSpeed = 2.0f;					//* 回転の速度

	Vector3 translation_;					//* 移動座標
	Vector2 destinationAngle_ {0.0f,0.0f};	//* 目標角度
	Vector3 rotation_;						//* 回転

	Matrix4x4 rotateMatrix_;			//* 回転行列
	Matrix4x4 worldMat_;				//* World行列
	Matrix4x4 viewMatrix_;				//* view行列
	Matrix4x4 viewProjectionMatrix_;	//* 渡す用の行列

};

