#pragma once
#include<vector>
#include"VertexData.h"
#include"Material.h"

struct ModelData{
    std::vector<VertexData> vertices;    // 頂点データのリスト
    std::vector<uint32_t> indices;       // インデックスデータのリスト
    MaterialData material;               // マテリアルデータ
};