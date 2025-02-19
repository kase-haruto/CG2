#pragma once
#include"lib/myMath/Vector3.h"
#include "lib/myMath/Quaternion.h"

// c++
#include <string>

struct EulerTransform{
	Vector3 scale;
	Vector3 rotate;
	Vector3 translate;

	void Initialize(){
		scale = {1.0f,1.0f,1.0f};
		rotate = {0.0f,0.0f,0.0f};
		translate = {0.0f,0.0f,0.0f};
	}

	void ShowImGui(const std::string& lavel = "Transform");
};

struct QuaternionTransform{
	Vector3 scale;
	Quaternion rotate;
	Vector3 translate;
};