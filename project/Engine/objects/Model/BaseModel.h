#pragma once

#include "engine/objects/ModelData.h"
#include "engine/graphics/Material.h"
#include "engine/objects/TransformationMatrix.h"
#include "engine/objects/Transform.h"
#include "lib/myMath/Vector4.h"

#include <d3d12.h>
#include <wrl.h>
#include <string>
#include <memory>

class BaseModel{
public:
    virtual ~BaseModel() = default;

    //=============
    // 仮想関数一覧
    //=============
    /// 初期化
    virtual void Initialize() = 0;
    /// モデルの作成や読み込み
    virtual void Create(const std::string& filename) = 0;
    /// 毎フレームの更新
    virtual void Update() = 0;
    virtual void AnimationUpdate(){}
    /// 行列更新
    virtual void UpdateMatrix() = 0;
    /// バッファへのMap操作(リソースへのCPU書き込みなど)
    virtual void Map() = 0;
    /// ImGuiの更新処理
    virtual void ShowImGuiInterface() = 0;
    /// 描画
    virtual void Draw() = 0;

    //=============
    // Transform関連
    //=============
    virtual void SetPos(const Vector3& pos) = 0;
    virtual void SetSize(const Vector3& size) = 0;
    virtual void SetUvScale(const Vector3& uvScale) = 0;
    virtual void SetColor(const Vector4& color) = 0;
    virtual const Vector4& GetColor() const = 0;

protected:
    //=============
    // DirectX関連
    //=============
    Microsoft::WRL::ComPtr<ID3D12Device>              device_;
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList_;
    Microsoft::WRL::ComPtr<ID3D12RootSignature>       rootSignature_;
    Microsoft::WRL::ComPtr<ID3D12PipelineState>       pipelineState_;

    //=============
    // リソース・バッファビュー
    //=============
    D3D12_VERTEX_BUFFER_VIEW vertexBufferView_ {};
    D3D12_INDEX_BUFFER_VIEW  indexBufferView_ {};

    Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_;
    Microsoft::WRL::ComPtr<ID3D12Resource> wvpResource_;

    //=============
    // テクスチャ
    //=============
    D3D12_GPU_DESCRIPTOR_HANDLE handle_ {};

    //=============
    // モデルデータ・マテリアル
    //=============
    std::shared_ptr<ModelData> modelData_;
    Material* materialData_ = nullptr;
    TransformationMatrix* matrixData_ = nullptr;
    int currentLightingMode_ = 0;
public:
    //=============
    // 各種パラメータ
    //=============
    Vector4    RGBa = {1.0f, 1.0f, 1.0f, 1.0f};
    EulerTransform  uvTransform {{1.0f, 1.0f, 1.0f},
                             {0.0f, 0.0f, 0.0f},
                             {0.0f, 0.0f, 0.0f}};
    EulerTransform  transform {{1.0f, 1.0f, 1.0f},
                           {0.0f, 0.0f, 0.0f},
                           {0.0f, 0.0f, 0.0f}};
    Matrix4x4  worldMatrix {};


public:
    EulerTransform* parent_;

    Material materialParameter_;

protected:
    /// リソースを生成する際のディレクトリパス(例)
    static const std::string directoryPath_;

    //============
    // バッファ作成/マッピング(派生先で使う)
    //============
    virtual void CreateMaterialBuffer() = 0;
    virtual void CreateMatrixBuffer() = 0;

    virtual void MaterialBufferMap() = 0;
    virtual void MatrixBufferMap() = 0;
};
