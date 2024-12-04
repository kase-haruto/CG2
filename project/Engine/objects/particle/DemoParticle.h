#pragma once

#include "ParticleSystem.h"
#include "particleBehavior/PtlBehavior_Diffusion.h"

class DemoParticle
    : public ParticleSystem, public PtlBehavior_Diffusion{
public:
    //===================================================================*/
    //                   public methods
    //===================================================================*/
    DemoParticle();
    ~DemoParticle() override = default;

    // 初期化処理
    void Initialize(const std::string& modelName, const std::string& textureFilePath,uint32_t count)override;

    // 更新処理
    void Update() override;
};