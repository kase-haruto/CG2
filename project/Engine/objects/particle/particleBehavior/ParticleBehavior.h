#pragma once

#include "../BaseParticle.h"

class ParticleBehavior{
public:
    virtual ~ParticleBehavior() = default;
    virtual void Initialize(BaseParticle& particle) = 0;
    virtual void ApplyBehavior(BaseParticle& particle) = 0;
};

