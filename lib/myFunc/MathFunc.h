#pragma once

#include"Vector3.h"
#include "Matrix4x4.h"

#include<vector>

/// <summary>
/// CatmullRom補完
/// </summary>
/// <param name="p0"></param>
/// <param name="p1"></param>
/// <param name="p2"></param>
/// <param name="p3"></param>
/// <param name="t"></param>
/// <returns></returns>
Vector3 CatmullRomInterpolation(
	const Vector3& p0, const Vector3& p1, const Vector3& p2, const Vector3& p3, float t);

Vector3 CatmullRomPosition(const std::vector<Vector3>& points, float t);


//// ビューポート変換行列
//Matrix4x4 MakeViewportMatrix(float l, float t, float w, float h, float minDepth, float maxDepth){
//	Matrix4x4 result;
//	result = {w / 2, 0, 0, 0, 0, -h / 2, 0, 0, 0, 0, maxDepth - minDepth, 0, l + (w / 2), t + (h / 2), minDepth, 1};
//	return result;
//}

//// 座標系変換
//Vector3 Transform(const Vector3& vector, const Matrix4x4& matrix){
//	Vector3 result = {0, 0, 0};
//
//	// 同次座標系への変換
//	// 変換行列を適用
//	Vector4 homogeneousCoordinate(
//		vector.x * matrix.m[0][0] + vector.y * matrix.m[1][0] + vector.z * matrix.m[2][0] + matrix.m[3][0],
//		vector.x * matrix.m[0][1] + vector.y * matrix.m[1][1] + vector.z * matrix.m[2][1] + matrix.m[3][1],
//		vector.x * matrix.m[0][2] + vector.y * matrix.m[1][2] + vector.z * matrix.m[2][2] + matrix.m[3][2],
//		vector.x * matrix.m[0][3] + vector.y * matrix.m[1][3] + vector.z * matrix.m[2][3] + matrix.m[3][3]);
//
//	// 同次座標系から3次元座標系に戻す
//	float w = homogeneousCoordinate.w;
//	assert(w != 0.0f); // wが0でないことを確認
//	result.x = homogeneousCoordinate.x / w;
//	result.y = homogeneousCoordinate.y / w;
//	result.z = homogeneousCoordinate.z / w;
//
//	return result;
//}