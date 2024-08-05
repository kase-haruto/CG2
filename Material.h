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
};

struct MaterialData{
	std::string textureFilePath;
	bool hasTexture;
};