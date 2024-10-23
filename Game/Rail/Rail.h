#pragma once

#include "Vector3.h"

#include <vector>

class Rail{
public:
	Rail(){}
	~Rail(){}


	void SetCtrlPoints(const std::vector<Vector3>& controlPoint){ ctrlPoints_ = controlPoint; }
	const std::vector<Vector3>& GetControlPoint()const{ return ctrlPoints_; }

private:
	//////////////////////////////////////////////////////////
	//		メンバ変数
	/////////////////////////////////////////////////////////
	std::vector<Vector3>ctrlPoints_;
};
