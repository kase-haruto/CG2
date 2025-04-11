#pragma once
#include "lib/mymath/Matrix4x4.h"

struct TransformationMatrix{
	Matrix4x4 WVP;
	Matrix4x4 world;
	Matrix4x4 WorldInverseTranspose;
};