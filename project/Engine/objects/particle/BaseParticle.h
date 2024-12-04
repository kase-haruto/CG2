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
    virtual ~BaseParticle() = default;

    virtual void Initialize(const std::string& modelName, const std::string& textureFilePath, uint32_t count);
    virtual void Update();
    virtual void Draw();

    void Emit(uint32_t count);

    bool IsAlive() const{ return instanceNum_ > 0; }


private:
    //===================================================================*/
    //                    private methods
    //===================================================================*/
    void CreateBuffer();
    void Map();
    void CreateSRV();

private:
    Matrix4x4 backToFrontMatrix_;

public:
    //===================================================================*/
    //                    public methods
    //===================================================================*/
    std::vector<ParticleData::Parameters> particles_;
    uint32_t kMaxInstanceNum_ = 512;
    uint32_t instanceNum_ = 0;

protected:
    //===================================================================*/
    //                    protected methods
    //===================================================================*/
    ParticleData::Emitter emitter_ {};
    /* resources =======================*/
    Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_ = nullptr;
    Microsoft::WRL::ComPtr<ID3D12Resource> vertexBuffer_ = nullptr;
    Microsoft::WRL::ComPtr<ID3D12Resource>instancingResource_ = nullptr;

    D3D12_VERTEX_BUFFER_VIEW vertexBufferView {};
    D3D12_GPU_DESCRIPTOR_HANDLE instancingSrvHandleGPU_;

    /* data =======================*/
    std::shared_ptr<ModelData> modelData_;
    Material* materialData = nullptr;
    ParticleData::ParticleForGPU* instancingData_ = nullptr;

    D3D12_GPU_DESCRIPTOR_HANDLE textureHandle_;
};