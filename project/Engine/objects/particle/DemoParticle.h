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

    void Initialize(const std::string& modelName) override;
    void Update() override;
};