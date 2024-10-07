#pragma once
#include "Vector3.h"
#include "Vector4.h"
#include "ViewProjection.h"
#include "Engine/objects/TransformationMatrix.h"

#include <array>
#include <cstdint>
#include <d3d12.h>
#include <memory>
#include <string>
#include <wrl.h>

// 基本プリミティブ描画
class PrimitiveDrawer{
private:
    template<class T> using ComPtr = ComPtr<T>;

public:
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

    /// <summary>
    /// コンストラクタ
    /// </summary>
    PrimitiveDrawer();

    /// <summary>
    /// デストラクタ
    /// </summary>
    ~PrimitiveDrawer();

    /// <summary>
    /// メッシュ生成
    /// </summary>
    /// <param name="vertexCount">頂点数</param>
    /// <param name="indexCount">インデックス数</param>
    /// <returns></returns>
    std::unique_ptr<Mesh> CreateMesh(UINT vertexCount, UINT indexCount);

    /// <summary>
    /// 初期化
    /// </summary>
    void Initialize();

    /// <summary>
    /// 終了処理
    /// </summary>
    void Finalize();

    /// <summary>
    /// 3D線分の描画
    /// </summary>
    /// <param name="p1">始点座標</param>
    /// <param name="p2">終点座標</param>
    /// <param name="color">色(RGBA)</param>
    void DrawLine3d(const Vector3& p1, const Vector3& p2, const Vector4& color);

    /// <summary>
    /// 実際に描画
    /// </summary>
    void Render();

    /// <summary>
    /// リセット
    /// </summary>
    void Reset();

    // wvpリソース
    void CreateMatrixBuffer();
    void UpdateMatrixBuffer();

    /// <summary>
    /// ビュープロジェクションのセット
    /// </summary>
    /// <param name="viewProjection"></param>
    void SetViewProjection(const ViewProjection* viewProjection){
        viewProjection_ = viewProjection;
    }

private:
    PrimitiveDrawer(const PrimitiveDrawer&) = delete;
    PrimitiveDrawer& operator=(const PrimitiveDrawer&) = delete;

    /// <summary>
    /// 各種メッシュ生成
    /// </summary>
    void CreateMeshes();

    // 線分
    std::unique_ptr<Mesh> line_;
    // 線分の使用インデックス
    uint32_t indexLine_ = 0;
    // 参照するビュープロジェクション
    const ViewProjection* viewProjection_ = nullptr;

    // Graphics settings
    ComPtr<ID3D12Device> device_ = nullptr;
    ComPtr<ID3D12GraphicsCommandList> commandList_ = nullptr;
    ComPtr<ID3D12RootSignature> rootSignature_ = nullptr;
    ComPtr<ID3D12PipelineState> pipelineState_ = nullptr;

    // wvp用リソース
    ComPtr<ID3D12Resource> wvpResource_ = nullptr;
    TransformationMatrix* matrixData_ = nullptr; // マッピングされたバッファのデータ
};
