#pragma once

#include "engine/objects/ParticleSystem/Particle.h"
#include "engine/objects/ParticleSystem/Emitter.h"
#include "engine/objects/ParticleSystem/ParticleBehavior.h"
#include "engine/graphics/SrvLocator.h"
#include "engine/objects/ModelData.h"
#include "engine/graphics/Material.h"

/* c++ */
#include <list>
#include <vector>
#include <unordered_map>
#include <memory>
#include <string>
#include <cstdint>
#include <d3d12.h>
#include <random>
#include <wrl.h>

/// <summary>
/// GPU 側データ構造
/// </summary>
struct ParticleForGPU{
    Matrix4x4 WVP;
    Matrix4x4 World;
    Vector4 color;
};

class ParticleManager{
public:
    explicit ParticleManager(uint32_t maxInstances);
    ~ParticleManager();

    // 初期化
    void Initialize();

    // パーティクル生成・更新・描画
    void Create();
    void Update();
    void Draw();

    // UI更新
    void UpdateUI();

    // 挙動の追加
    void AddBehavior(std::unique_ptr<ParticleBehavior> behavior);

    // テクスチャの追加
    void AddTexture(const std::string& name);

private:
    // パーティクル生成
    Particle MakeNewParticle(std::mt19937& randomEngine, const Vector3& translate);
    std::list<Particle> Emit(const Emitter& emitter, std::mt19937& randomEngine);

    // バッファ作成・リソースマッピング
    void CreateBuffer();
    void CreateVertexBuffer();
    void CreateMaterialBuffer();
    void CreateMatrixBuffer();
    void MapResources();
    void MapVertexBuffer();
    void MapMaterialBuffer();
    void MapMatrixBuffer();

    // DirectX関連
    Microsoft::WRL::ComPtr<ID3D12Device> device_;
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList_;
    Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_;
    Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState_;

    D3D12_VERTEX_BUFFER_VIEW vertexBufferView_;
    Microsoft::WRL::ComPtr<ID3D12Resource> instancingResource_;
    Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_;
    Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_;

    uint32_t maxInstances_;
    uint32_t numActiveInstances_ = 0;

    // モデルデータ
    std::shared_ptr<ModelData> modelData_;
    Material* materialData_ = nullptr;
    ParticleForGPU* instancingData_ = nullptr;

    // パーティクルデータ
    std::list<Particle> particles_;
    std::vector<std::unique_ptr<ParticleBehavior>> behaviors_;
    Emitter emitter_;

    // 色設定
    Vector4 baseColor_;

    // テクスチャ
    std::unordered_map<std::string, D3D12_GPU_DESCRIPTOR_HANDLE> textures_;
};
