#pragma once

#include <vector>
#include <d3d12.h>
#include <wrl.h>

#include "engine/graphics/Material.h"
#include "engine/objects/TransformationMatrix.h"
#include "lib/myMath/Vector3.h"
#include "lib/myMath/Vector4.h"

/// <summary>
/// トレイル用の頂点 (ワールド座標を直接持つ想定)
/// </summary>
struct TrailVertex{
    Vector4 position;
    Vector4 color;   // フェード用アルファ
    Vector3 normal;
};

/// <summary>
/// 剣の軌跡(トレイル)描画クラス
/// Modelクラスと同じくMaterial/Matrixを使って描画
/// </summary>
class SwordTrail{
public:
    SwordTrail() = default;
    ~SwordTrail() = default;

    /// 初期化 (GPUリソース生成など)
    void Initialize(
        Microsoft::WRL::ComPtr<ID3D12Device> device,
        Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> cmdList,
        Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSig,
        Microsoft::WRL::ComPtr<ID3D12PipelineState> pso
    );

    /// 剣の先端と根元を追加 (2頂点/フレーム)
    void AddSegment(const Vector3& tip, const Vector3& base);

    /// フレーム更新 (アルファフェード → 削除)
    void Update(float deltaTime);

    /// 描画
    void Draw();

    /// 全消去
    void Clear();

    /// フェード速度 / 最小アルファ
    void SetFadeSpeed(float speed){ fadeSpeed_ = speed; }
    void SetMinAlpha(float alpha){ minAlpha_ = alpha; }

private:
    /// リソース生成
    void CreateVertexBuffer();
    void CreateMaterialBuffer();
    void CreateMatrixBuffer();

    /// Map (CPU→GPU)
    void Map();
    void VertexBufferMap();      //< 毎フレーム呼ぶ(UpdateVertexBuffer)的にも可
    void MaterialBufferMap();
    void MatrixBufferMap();

    /// ViewProjectionを掛け合わせる
    void UpdateMatrix();

    /// 頂点配列→GPU反映
    void UpdateVertexBuffer();

private:
    // D3D12関連
    Microsoft::WRL::ComPtr<ID3D12Device>              device_;
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList_;
    Microsoft::WRL::ComPtr<ID3D12RootSignature>       rootSignature_;
    Microsoft::WRL::ComPtr<ID3D12PipelineState>       pipelineState_;

    // 頂点バッファ
    Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_;
    D3D12_VERTEX_BUFFER_VIEW               vbView_ {};
    TrailVertex* mappedVertices_ = nullptr;

    // Material & Matrix
    Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_;
    Microsoft::WRL::ComPtr<ID3D12Resource> matrixResource_;
    Material* materialData_ = nullptr;
    TransformationMatrix* matrixData_ = nullptr;

    // 頂点リスト
    std::vector<TrailVertex> vertices_;
    static const size_t MAX_TRAIL_VERTICES = 400; // 容量(例)

    // フェード
    float fadeSpeed_ = 1.0f;  // 1秒あたり alpha が1.0減衰
    float minAlpha_ = 0.05f; // 削除閾値

    // テクスチャ (例: uvChecker.png)
    D3D12_GPU_DESCRIPTOR_HANDLE textureHandle_ {};
};
