#pragma once

#include "lib/myMath/Vector3.h"
#include "lib/myMath/Matrix4x4.h"


#include <array>

struct OBB{
	Vector3 size;
	Vector3 rotate;
	Vector3 center;

	// 8頂点を返す関数
	std::array<Vector3, 8> GetVertices() const;

	void Draw();
};

struct Sphere{
	Vector3 center;
	float radius;

	void Draw();
};