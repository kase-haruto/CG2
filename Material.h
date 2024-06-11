#pragma once
#include"Vector4.h"
#include<stdint.h>

struct Material{
	Vector4 color;
	int32_t enableLighting;
};