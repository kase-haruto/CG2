#pragma once
#include"Vector4.h"
#include"Mat3.h"
#include"Matrix4x4.h"

#include<stdint.h>
#include<string>

struct Material{
	Vector4 color;
	int32_t enableLighting;
	float pad[3];
	Matrix4x4 uvTransform;
	float shininess;
};

struct MaterialData{
	std::string textureFilePath;
	Vector3 uv_scale;
	Vector3 uv_offset;
	Vector3 uv_translate;
};