#pragma once

#include "ModelData.h"
#include "Material.h"
#include "TransformationMatrix.h"
#include "Transform.h"
#include "ViewProjection.h"
#include "Vector4.h"
#include <cstdint>
#include <d3d12.h>
#include <wrl.h>

class DirectXCommon;

class Particle{
public:
    Particle();
    ~Particle();

    // 初期化処理
    void Initialize(ViewProjection* viewProjection);
    // 更新処理
    void Update();
    // 描画処理
    void Draw();
    // バッファの作成
    void CreateBuffer();
    // リソースのマッピング
    void Map();
    // モデルデータを取得
    ModelData GetModelData() const{ return modelData; }

    // SRVの作成
    void CreateSRV();

private:
    // バッファ作成メソッド
    void CreateVertexBuffer();
    void CreateMaterialBuffer();
    void CreateMatrixBuffer();
    // リソースマッピングメソッド
    void VertexBufferMap();
    void MaterialBufferMap();
    void MatrixBufferMap();

private:
    // DirectX関連
    Microsoft::WRL::ComPtr<ID3D12Device> device_;
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList_;
    Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_;
    Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState_;

    // リソース
    D3D12_VERTEX_BUFFER_VIEW vertexBufferView {};
    Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_;
    Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_;
    Microsoft::WRL::ComPtr<ID3D12Resource> instancingResource_;

    // 色設定
    Vector4 RGBa;

    // インスタンス数
    static constexpr uint32_t kNumInstance = 10;

    // トランスフォーム情報
    Transform transforms[kNumInstance];

    // モデルデータ
    ModelData modelData;

    // マテリアルデータ
    Material* materialData = nullptr;

    // インスタンシングデータ
    TransformationMatrix* instancingData = nullptr;

    // ビュー・プロジェクション
    ViewProjection* viewProjection_ = nullptr;

    // テクスチャハンドル
    D3D12_GPU_DESCRIPTOR_HANDLE handle;

    // インスタンシングSRVのGPUハンドル
    D3D12_GPU_DESCRIPTOR_HANDLE instancingSrvHandleGPU_;
};
