#pragma once

#include "engine/objects/ModelData.h"
#include "engine/graphics/Material.h"
#include "engine/objects/TransformationMatrix.h"
#include "engine/objects/Transform.h"

/* math */
#include "lib/myMath/Vector4.h"
#include "engine/physics/AABB.h"
#include "Engine/physics/Shape.h"
#include "lib/myFunc/Random.h"

/* c++ */
#include <list>
#include <vector>
#include <cstdint>
#include <memory>
#include <d3d12.h>
#include <wrl.h>

namespace ParticleData{
    struct Parameters{
        EulerTransform transform {{1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f}};
        Vector3 velocity {};
        Vector4 color {1.0f, 1.0f, 1.0f, 1.0f};
        float lifeTime = 1.0f;
        float currentTime = 0.0f;
		float maxScale = 1.0f;

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
        EulerTransform transform {{1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f}};    //エミッタのtransform
        uint32_t count;             //発生数
        float frequency;            //発生頻度
        float frequencyTime = 0;    //頻度用時刻

        void Initialize(uint32_t count);
        void Initialize(const EulerTransform& transform, const float frequency, const float frequencyTime, uint32_t count);
    };
}

enum class EmitterShape{
    OBB,
    Sphere
};

class BaseParticle{
public:
    //===================================================================*/
    //                    public methods
    //===================================================================*/
    BaseParticle();
    virtual void Initialize(const std::string& modelName,const std::string& texturePath, const uint32_t count);
    virtual ~BaseParticle() = default;
    virtual void Update();
    virtual void Draw();

    virtual void ImGui();

    virtual void Emit(uint32_t count);

    // ライフタイム設定用の仮想関数
    virtual float SetParticleLifeTime() const{ return Random::Generate(0.5f, 1.0f); }


    // 派生クラスでオーバーライド可能な速度生成
    virtual Vector3 GenerateVelocity(float speed);

    virtual bool GetUseRandomColor() const{ return true; } // デフォルトではランダム使用
    virtual Vector4 GetSelectedColor() const{ return Vector4(1.0f, 1.0f, 1.0f, 1.0f); }


    // maxScale設定のためのインターフェース
    void SetUseRandomScale(bool useRandom){ useRandomScale_ = useRandom; }
    void SetFixedMaxScale(float scale){ fixedMaxScale_ = scale; }
    void SetRandomScaleRange(float minScale, float maxScale){
        randomScaleMin_ = minScale;
        randomScaleMax_ = maxScale;
    }

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
	bool isStatic_ = false;
	bool autoEmit_ = true;
    int32_t kMaxInstanceNum_ = 256;
    int32_t instanceNum_ = 0;


    bool useRandomScale_ = true;         // ランダムスケールを使用するかのフラグ
    float fixedMaxScale_ = 1.0f;        // 固定スケール値
    float randomScaleMin_ = 1.0f;       // ランダムスケールの最小値
    float randomScaleMax_ = 6.0f;       // ランダムスケールの最大値
protected:
    //===================================================================*/
    //                    protected methods
    //===================================================================*/
   

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


    /* emitter ------------------------------------*/
    ParticleData::Emitter emitter_ {};

    bool emitPosX_ = true;
    bool emitNegX_ = true;
    bool emitPosY_ = true;
    bool emitNegY_ = true;
    bool emitPosZ_ = true;
    bool emitNegZ_ = true;

    bool isFixationAlpha_ = false;
    bool isBillboard_ = true;

    // 形状選択用
    EmitterShape currentShape_ = EmitterShape::OBB;

};
