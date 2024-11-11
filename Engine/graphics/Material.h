#pragma once
#include"lib/myMath/Vector2.h"
#include"lib/myMath/Vector3.h"
#include"lib/myMath/Vector4.h"
#include"lib/myMath/Mat3.h"
#include"lib/myMath/Matrix4x4.h"

/* c++ */
#include<stdint.h>
#include<string>

struct Material{
	Vector4 color;
	int32_t enableLighting;
	float pad[3];
	Matrix4x4 uvTransform;
	float shininess;
};

struct Material2D{
	Vector4 color;
	Matrix4x4 uvTransform;
};

struct MaterialData{
	std::string textureFilePath;
	Vector3 uv_scale;
	Vector3 uv_offset;
	Vector3 uv_translate;
};