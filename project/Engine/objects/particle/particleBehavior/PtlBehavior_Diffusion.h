#pragma once
#include "ParticleBehavior.h"

class PtlBehavior_Diffusion :
    public ParticleBehavior{
public:
    //===================================================================*/
    //                    public methods
    //===================================================================*/
    ~PtlBehavior_Diffusion()override = default;

    void Initialize(BaseParticle& particle)override;

    void ApplyBehavior(BaseParticle& particle)override;
};

