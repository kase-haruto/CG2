#pragma once
#include "ParticleBehavior.h"

class PtlBehavior_Diffusion :
    public ParticleBehavior{
public:
    //===================================================================*/
    //                    public methods
    //===================================================================*/
    ~PtlBehavior_Diffusion()override = default;

    void ApplyBehavior(ParticleSystem& particle)override;
};

