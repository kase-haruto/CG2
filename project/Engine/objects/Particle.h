﻿#pragma once

#include "engine/objects/ModelData.h"
#include "engine/graphics/Material.h"
#include "engine/objects/TransformationMatrix.h"
#include "engine/objects/Transform.h"

/* math */
#include "lib/myMath/Vector4.h"
#include "engine/physics/AABB.h"

/* c++ */
#include <list>
#include <cstdint>
#include <d3d12.h>
#include <wrl.h>
#include <memory>


/// <summary>
/// フィールド
/// </summary>
struct AccelerationField{
    Vector3 acceleration;
    AABB area;
    bool isUpdate;
};

/// <summary>
/// パーティクル
/// </summary>
struct Particle{
    Transform transform;
    Vector3 velocity;
    Vector4 color;
    float lifeTime;
    float currentTime;
};

/// <summary>
/// エミッター
/// </summary>
struct Emitter{
    Transform transform;    //エミッタのtransform
    uint32_t count;         //発生数
    float frequency;        //発生頻度
    float frequencyTime;    //頻度用時刻
};


/// <summary>
/// gpu側
/// </summary>
struct ParticleForGPU{
    Matrix4x4 WVP;
    Matrix4x4 World;
    Vector4 color;
};

class DirectXCommon;

class ParticleManager{
public:
    ParticleManager(const uint32_t kInstanceNum);
    ParticleManager() = default;
    ~ParticleManager();

    /// <summary>
    /// 初期化
    /// </summary>
    /// <param name="viewProjection"></param>
    void Initialize();
    
    /// <summary>
    /// パーティクルの生成と初期化
    /// </summary>
    /// <param name="numInstance"></param>
    /// <param name="viewProjection"></param>
    void Create();
   
    /// <summary>
    /// 新しいパーティクルの生成
    /// </summary>
    /// <param name="randomEngine"></param>
    /// <returns></returns>
    Particle MakeNewParticle(const Vector3& translate);
   
    /// <summary>
    /// 更新処理
    /// </summary>
    void Update();
   
    /// <summary>
    /// 描画
    /// </summary>
    void Draw();

    /// <summary>
    /// エミッター関数
    /// </summary>
    std::list<Particle> Emit(const Emitter& emitter);
   
    // バッファの作成
    void CreateBuffer();
    // リソースのマッピング
    void Map();
    // モデルデータを取得
    std::shared_ptr<ModelData> GetModelData() const{ return modelData; }

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
    Microsoft::WRL::ComPtr<ID3D12Resource>instancingResource_;
    Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_;
    Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_;

    // 色設定
    Vector4 RGBa;

    //パーティクルの最大個数
    const uint32_t kMaxInstance_;

    //描画する個数
    uint32_t numInstance_;

    //パーティクル
    std::list<Particle> particle_;

    //エミッター
    Emitter emitter {};

    //フィールド
    AccelerationField accelerationField;

    //反対側に回す回転行列
    Matrix4x4 backToFrontMatrix_;

    // モデルデータ
    std::shared_ptr<ModelData> modelData;

    // マテリアルデータ
    Material* materialData = nullptr;

    // インスタンシングデータ
    ParticleForGPU* instancingData = nullptr;

    // テクスチャハンドル
    D3D12_GPU_DESCRIPTOR_HANDLE handle;

    // インスタンシングSRVのGPUハンドル
    D3D12_GPU_DESCRIPTOR_HANDLE instancingSrvHandleGPU_;
};