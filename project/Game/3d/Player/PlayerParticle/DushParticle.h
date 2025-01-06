#pragma once

#include "Engine/objects/particle/ParticleSystem.h"

class DushParticle :
    public ParticleSystem{
public:
    //===================================================================*/
    //                   public methods
    //===================================================================*/
    DushParticle();
    ~DushParticle() override = default;

    void Initialize(const std::string& modelName, const std::string& texturePath) override;
    void Update() override;

private:
    //===================================================================*/
    //                   private methods
    //===================================================================*/
    uint32_t particleNum_;
};
