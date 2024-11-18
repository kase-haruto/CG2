#pragma once

/* engine */
#include"../graphics/VertexData.h"
#include"../graphics/Material.h"

/* c++ */
#include<vector>

struct ModelData{
    std::vector<VertexData> vertices;    // 頂点データのリスト
    std::vector<uint32_t> indices;       // インデックスデータのリスト
    MaterialData material;               // マテリアルデータ
};