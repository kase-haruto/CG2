#pragma once
#include"lib/myMath/Vector3.h"
#include "lib/myMath/Quaternion.h"

struct EulerTransform{
	Vector3 scale;
	Vector3 rotate;
	Vector3 translate;
};

struct QuaternionTransform{
	Vector3 scale;
	Quaternion rotate;
	Vector3 translate;
};