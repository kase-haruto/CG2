#pragma once
#include <Engine/Foundation/Math/Vector2.h>
#include <Engine/Foundation/Math/Vector3.h>
#include <Engine/Foundation/Math/Vector4.h>

struct VertexData{
	Vector4 position;
	Vector2 texcoord;
	Vector3 normal;
};

struct VertexPosColor{
	Vector3 pos;   // xyz座標
	Vector4 color; // RGBA
};