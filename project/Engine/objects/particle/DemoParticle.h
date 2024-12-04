#pragma once

#include "ParticleSystem.h"
#include "particleBehavior/PtlBehavior_Diffusion.h"

//* c++

class DemoParticle 
    : public ParticleSystem, public PtlBehavior_Diffusion{

public:
    //===================================================================*/
    //                   public methods
    //===================================================================*/
    DemoParticle();
    ~DemoParticle() override = default;

    void Initialize(const std::string& modelName, const std::string& texturePath) override;
    void Update() override;

private:
    //===================================================================*/
    //                   private methods
    //===================================================================*/
    uint32_t particleNum_;
};