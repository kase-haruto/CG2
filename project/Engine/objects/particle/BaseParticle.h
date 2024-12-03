#pragma once

#include "engine/objects/ModelData.h"
#include "engine/graphics/Material.h"
#include "engine/objects/TransformationMatrix.h"
#include "engine/objects/Transform.h"

/* math */
#include "lib/myMath/Vector4.h"
#include "engine/physics/AABB.h"

/* c++ */
#include <list>
#include <vector>
#include <cstdint>
#include <memory>
#include <d3d12.h>
#include <wrl.h>

namespace ParticleData{
    struct Parameters{
        Transform transform {{1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f}};
        Vector3 velocity {};
        Vector4 color {1.0f, 1.0f, 1.0f, 1.0f};
        float lifeTime = 1.0f;
        float currentTime = 0.0f;

        void SetColorRandom();          // 色をランダムで初期化
        void SetColorInitialize();      // 色を初期化する(白)

        void SetVelocityRandom(float min, float max);
    };

    struct ParticleForGPU{
        Matrix4x4 WVP;
        Matrix4x4 World;
        Vector4 color;
    };

    struct Emitter{
        Transform transform {{1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f}};    //エミッタのtransform
        uint32_t count;         //発生数
        float frequency;        //発生頻度
        float frequencyTime;    //頻度用時刻

        void Initialize(uint32_t count);
        void Initialize(const Transform& transform, const float frequency, const float frequencyTime, uint32_t count);
    };
}


class BaseParticle{
public:
    //===================================================================*/
    //                    public methods
    //===================================================================*/
    BaseParticle();
    virtual void Initialize(const std::string& modelName, const uint32_t count);
    virtual ~BaseParticle() = default;
    virtual void Update();
    virtual void Draw();

    virtual void Emit(uint32_t count);


private:
    //===================================================================*/
    //                    private methods
    //===================================================================*/



    /* resources =======================*/
    void CreateBuffer();
    void Map();
    void CreateSRV();

public:
    //===================================================================*/
    //                    public methods
    //===================================================================*/
    std::vector<ParticleData::Parameters> particles_;
   
    int32_t kMaxInstanceNum_ = 500;
    int32_t instanceNum_ = 50;          //デフォルトとして50    

protected:
    //===================================================================*/
    //                    protected methods
    //===================================================================*/
    ParticleData::Emitter emitter_ {};

    D3D12_GPU_DESCRIPTOR_HANDLE textureHandle; // テクスチャハンドル

    //反対側に回す回転行列
    Matrix4x4 backToFrontMatrix_;

    /* resources =======================*/
    Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_ = nullptr;
    Microsoft::WRL::ComPtr<ID3D12Resource> vertexBuffer_ = nullptr;
    D3D12_VERTEX_BUFFER_VIEW vertexBufferView {};
    Microsoft::WRL::ComPtr<ID3D12Resource>instancingResource_ = nullptr;

    /* data =======================*/
    std::shared_ptr<ModelData> modelData_;
    Material* materialData = nullptr;

    // インスタンシングSRVのGPUハンドル
    D3D12_GPU_DESCRIPTOR_HANDLE instancingSrvHandleGPU_;

    // インスタンシングデータ
    ParticleData::ParticleForGPU* instancingData = nullptr;
};
