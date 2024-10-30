#pragma once
#include "Vector3.h"

/// <summary>3
/// 4x4行列
/// </summary>
struct Matrix4x4 final{
	float m[4][4];

	static Matrix4x4 Multiply(const Matrix4x4& m1, const Matrix4x4& m2);
	static Matrix4x4 MakeIdentity();
	// 逆行列
	static Matrix4x4 Inverse(const Matrix4x4& m);

	static Vector3 Transform(const Vector3& vector, const Matrix4x4& matrix);

	static Matrix4x4 MakeViewportMatrix(float l, float t, float w, float h, float minDepth, float maxDepth);
};
