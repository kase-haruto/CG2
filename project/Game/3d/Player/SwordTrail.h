#pragma once

#include <vector>
#include <d3d12.h>
#include <wrl.h>

#include "engine/graphics/Material.h"
#include "engine/objects/TransformationMatrix.h"
#include "lib/myMath/Vector3.h"
#include "lib/myMath/Vector4.h"

/// <summary>
/// 剣の軌跡(トレイル)描画クラス (TriangleStrip版)
/// </summary>
class SwordTrail{
    struct EffectVertexData{
        Vector4 position;
        Vector2 texcoord;
        Vector4 color;
    };

public:
    SwordTrail() = default;
    ~SwordTrail() = default;

    void Initialize(
        Microsoft::WRL::ComPtr<ID3D12Device> device,
        Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> cmdList,
        Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSig,
        Microsoft::WRL::ComPtr<ID3D12PipelineState> pso
    );

    /// <summary>剣の先端と根元を追加 (1フレームあたり2頂点)</summary>
    void AddSegment(const Vector3& tip, const Vector3& base);

    /// <summary>フレーム更新 (アルファフェード → 削除)</summary>
    void Update(float deltaTime);

    /// <summary>描画</summary>
    void Draw();

    /// <summary>全消去</summary>
    void Clear();

    void SetFadeSpeed(float speed){ fadeSpeed_ = speed; }
    void SetMinAlpha(float alpha){ minAlpha_ = alpha; }

private:
    void CreateVertexBuffer();
    void CreateMaterialBuffer();
    void CreateMatrixBuffer();

    void Map();
    void VertexBufferMap();
    void MaterialBufferMap();
    void MatrixBufferMap();

    void UpdateMatrix();
    void UpdateVertexBuffer();

private:
    Microsoft::WRL::ComPtr<ID3D12Device>              device_;
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList_;
    Microsoft::WRL::ComPtr<ID3D12RootSignature>       rootSignature_;
    Microsoft::WRL::ComPtr<ID3D12PipelineState>       pipelineState_;

    // 頂点バッファ
    Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_;
    D3D12_VERTEX_BUFFER_VIEW               vbView_ {};
    EffectVertexData* mappedVertices_ = nullptr;

    // Material & Matrix
    Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_;
    Microsoft::WRL::ComPtr<ID3D12Resource> matrixResource_;
    Material* materialData_ = nullptr;
    TransformationMatrix* matrixData_ = nullptr;

    // 頂点リスト (TriangleStrip用に [tip0, base0, tip1, base1, tip2, base2, ...] の順)
    std::vector<EffectVertexData> vertices_;
    static const size_t MAX_TRAIL_VERTICES = 400;

    // フェード
    float fadeSpeed_ = 1.0f;
    float minAlpha_ = 0.05f;

    // テクスチャ
    D3D12_GPU_DESCRIPTOR_HANDLE textureHandle_ {};

    // テクスチャ座標の進め方
    int   segmentCount_ = 0;
    float uvStep_ = 1.0f;
};
