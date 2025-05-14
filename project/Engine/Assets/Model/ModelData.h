#pragma once

/* engine */
#include <Engine/Assets/Animation/AnimationStruct.h>
#include <Engine/Graphics/Buffer/DxIndexBuffer.h>
#include <Engine/Graphics/Buffer/DxVertexBuffer.h>
#include <Engine/Graphics/Material.h>
#include <Engine/Renderer/Mesh/VertexData.h>

/* c++ */
#include <d3d12.h>
#include <map>
#include <vector>
#include <wrl.h>

struct ModelData{
	std::vector<VertexData> vertices;
	std::vector<uint32_t> indices;

	MaterialData material;

	//-----------------------------------------------------------
	// アニメーション情報
	//-----------------------------------------------------------
	std::map<std::string, JointWeightData> skinClusterData;
	Animation animation;
	Skeleton skeleton;
	// std::vector<Animation> animations;
	DxVertexBuffer<VertexData> vertexBuffer;
	DxIndexBuffer<uint32_t> indexBuffer;
};
