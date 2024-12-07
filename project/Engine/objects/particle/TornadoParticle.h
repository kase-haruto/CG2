#pragma once
#include "ParticleSystem.h"


class TornadoParticle :
    public ParticleSystem{

public:
    //===================================================================*/
    //                   public methods
    //===================================================================*/
    TornadoParticle();
    ~TornadoParticle()override = default;

    void Initialize(const std::string& modelName,const std::string& texturePath)override;
    void Update()override;

private:
    //===================================================================*/
    //                   private methods
    //===================================================================*/
    uint32_t particleNum_;
};
