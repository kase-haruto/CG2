#pragma once
#include "engine/objects/ModelData.h"
#include "engine/graphics/Material.h"
#include "engine/objects/TransformationMatrix.h"
#include "engine/objects/Transform.h"
#include "lib/myMath/Vector4.h"
#include "Engine/graphics/ViewProjection.h"

#include <d3d12.h>
#include <wrl.h>
#include <string>
#include <memory>

/// <summary>
/// モデル
/// </summary>
class Model{
public:
    Model() = default;
    Model(const std::string& fileName);
    ~Model();

    /// <summary>
    /// 初期化
    /// </summary>
    void Initialize();
    /// <summary>
    /// モデルの作成
    /// </summary>
    void Create(const std::string& filename);
    /// <summary>
    /// 更新
    /// </summary>
    void Update();

    void UpdateMatrix();

    void Map();

    /// <summary>
    /// imguiの更新
    /// </summary>
    void ShowImGuiInterface();
    /// <summary>
    /// 描画
    /// </summary>
    void Draw();

    void SetViewProjection(const ViewProjection* viewPro);

    void SetPos(const Vector3& pos){ transform.translate = pos; }

    void SetSize(const Vector3& size){ transform.scale = size; }

    void SetUvScale(const Vector3& uvScale){ uvTransform.scale = uvScale; }

    void SetColor(const Vector4& color){ RGBa = color; }

private:
    void CreateMaterialBuffer();
    void CreateMatrixBuffer();

    void MaterialBufferMap();
    void MatrixBufferMap();

private:
    /// DirectX 関連
    Microsoft::WRL::ComPtr<ID3D12Device> device_;
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList_;
    Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_;
    Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState_;

    /// Resources
    D3D12_VERTEX_BUFFER_VIEW vertexBufferView {};
    D3D12_INDEX_BUFFER_VIEW indexBufferView {};
    Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_;
    Microsoft::WRL::ComPtr<ID3D12Resource> wvpResource_;

    /// Texture
    D3D12_GPU_DESCRIPTOR_HANDLE handle;

    Vector4 RGBa;
    Transform uvTransform {{1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}};

    std::shared_ptr<ModelData> modelData;
    Material* materialData;

    TransformationMatrix* matrixData;

    int currentLightingMode = 0;

public:
    Material materialParameter;

    Transform transform;

    Matrix4x4 worldMatrix;

private:
    static const std::string directoryPath_;

public:
    const ViewProjection* viewProjection_;

};