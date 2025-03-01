#pragma once

/* engine */
#include"../graphics/VertexData.h"
#include"../graphics/Material.h"
#include"../objects/Animation/AnimationStruct.h"
#include "Engine/core/DirectX/Buffer/DxIndexBuffer.h"
#include "Engine/core/DirectX/Buffer/DxVertexBuffer.h"

/* c++ */
#include<vector>
#include <d3d12.h>
#include <wrl.h>

struct ModelData{
    std::vector<VertexData> vertices;
    std::vector<uint32_t> indices;

    MaterialData material;

    //-----------------------------------------------------------
    // アニメーション情報 (単体例)
    //-----------------------------------------------------------
    Animation animation;

    // あるいは複数アニメーション対応例:
    // std::vector<Animation> animations;
	DxVertexBuffer<VertexData> vertexBuffer;
	DxIndexBuffer<uint32_t> indexBuffer;
};
