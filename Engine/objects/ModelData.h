#pragma once
#include<vector>
#include"VertexData.h"
#include"Material.h"
struct ModelData{
	std::vector<VertexData> vertices;
	MaterialData material;
};