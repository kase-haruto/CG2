#pragma once

//* c++
#include <d3d12.h>
#include <wrl.h>
#include <cstdint>

// メッシュ
struct Mesh{
    template<class T> using ComPtr = ComPtr<T>;

    // 頂点バッファ
    ComPtr<ID3D12Resource> vertBuff;
    // インデックスバッファ
    ComPtr<ID3D12Resource> indexBuff;
    // 頂点バッファビュー
    D3D12_VERTEX_BUFFER_VIEW vbView {};
    // インデックスバッファビュー
    D3D12_INDEX_BUFFER_VIEW ibView {};
    // 頂点バッファマップ
    VertexPosColor* vertMap = nullptr;
    // インデックスバッファマップ
    uint16_t* indexMap = nullptr;
};