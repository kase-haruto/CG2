#pragma once
#include "lib/myMath/Vector3.h"
#include "lib/myMath/Vector4.h"
#include "engine/graphics/ViewProjection.h"
#include "Engine/objects/TransformationMatrix.h"

/* c++ */
#include <array>
#include <cstdint>
#include <memory>
#include <string>

// 基本プリミティブ描画
class PrimitiveDrawer{
private:
    template<class T> using ComPtr = ComPtr<T>;

    PrimitiveDrawer();

public:
    // インスタンスの取得
    static PrimitiveDrawer* GetInstance(){
        static PrimitiveDrawer instance;
        return &instance;
    }

    // デストラクタ
    ~PrimitiveDrawer();

    // コピーコンストラクタと代入演算子を禁止
    PrimitiveDrawer(const PrimitiveDrawer&) = delete;
    PrimitiveDrawer& operator=(const PrimitiveDrawer&) = delete;

    // 線分の最大数
    static const UINT kMaxLineCount = 4096;
    // 線分の頂点数
    static const UINT kVertexCountLine = 2;
    // 線分のインデックス数
    static const UINT kIndexCountLine = 0;

    // 頂点データ構造体
    struct VertexPosColor{
        Vector3 pos;   // xyz座標
        Vector4 color; // RGBA
    };

    // メッシュ
    struct Mesh{
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

    std::unique_ptr<Mesh> CreateMesh(UINT vertexCount, UINT indexCount);
    void Initialize();
    void Finalize();
    void DrawLine3d(const Vector3& p1, const Vector3& p2, const Vector4& color);
    void Render();
    void Reset();

    // wvpリソース
    void CreateMatrixBuffer();
    void UpdateMatrixBuffer();

    void SetViewProjection(const ViewProjection* viewProjection){
        viewProjection_ = viewProjection;
    }

private:
    void CreateMeshes();

    std::unique_ptr<Mesh> line_;
    uint32_t indexLine_ = 0;
    const ViewProjection* viewProjection_ = nullptr;

    ComPtr<ID3D12Device> device_ = nullptr;
    ComPtr<ID3D12GraphicsCommandList> commandList_ = nullptr;
    ComPtr<ID3D12RootSignature> rootSignature_ = nullptr;
    ComPtr<ID3D12PipelineState> pipelineState_ = nullptr;

    ComPtr<ID3D12Resource> wvpResource_ = nullptr;
    TransformationMatrix* matrixData_ = nullptr;
};
