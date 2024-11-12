#pragma once

#include "Transform.h"
#include "myFunc/MathFunc.h"
#include "Vector3.h"

#include "Matrix4x4.h"

#include <vector>

class RailCamera{
public:
	RailCamera(){}
	~RailCamera(){}

	void Initialize();

	void Update();

	void SetCtrlPoints(const std::vector<Vector3>& controlPoint){ ctrlPoints_ = controlPoint; }
	const std::vector<Vector3>& GetControlPoint()const{ return ctrlPoints_; }

	const Transform& GetTransform()const{ return transform_; }

	const Matrix4x4& GetWorldMat()const{ return worldMat_; }

	bool GetIsFinished()const{ return isFinishedRail_; }

	void SetIsFinished(const bool frag){ isFinishedRail_ = frag; }

	bool GetIsPaused()const{ return isPaused_; }

private:
	//////////////////////////////////////////////////////////
	//		メンバ変数
	/////////////////////////////////////////////////////////
	std::vector<Vector3>ctrlPoints_;

	Transform transform_;

	Matrix4x4 worldMat_;

	float t_;

	bool isPaused_ = false;       // 停止状態フラグ
	bool pauseComplete_ = false;
	float pauseTimer_ = 0.0f;     // 停止時間のカウント
	const float pauseDuration_ = 5.0f; // 停止時間（8秒）

	bool isFinishedRail_ = false;

};
