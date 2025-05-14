#pragma once

struct Matrix4x4;

struct Vector4 final{
	float x;
	float y;
	float z;
	float w;

	static Vector4 TransformVector(const Matrix4x4& m, const Vector4& v);
};