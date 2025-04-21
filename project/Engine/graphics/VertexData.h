#pragma once
#include "lib/myMath/Vector2.h"
#include "lib/myMath/Vector4.h"
#include "lib/myMath/Vector3.h"

struct VertexData{
	Vector4 position;
	Vector2 texcoord;
	Vector3 normal;
};

struct VertexPosColor{
	Vector3 pos;   // xyz座標
	Vector4 color; // RGBA
};