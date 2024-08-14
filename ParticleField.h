#pragma once
#include"Vector3.h"
#include"AABB.h"

/// <summary>
/// フィールド
/// </summary>
struct AccelerationField{
	Vector3 acceleration;
	AABB area;
	bool isUpdate;
};