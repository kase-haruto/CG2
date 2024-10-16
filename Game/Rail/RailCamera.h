#pragma once

#include "Transform.h"
#include "myFunc/MathFunc.h"
#include "Vector3.h"

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

private:
	//////////////////////////////////////////////////////////
	//		メンバ変数
	/////////////////////////////////////////////////////////
	std::vector<Vector3>ctrlPoints_;

	Transform transform_;

	float t_;
};
