#pragma once
#include "engine/objects/ParticleSystem/Particle.h"

// 挙動の抽象クラス
class ParticleBehavior{
public:
    virtual void Update(Particle& particle, float deltaTime) = 0;
    virtual ~ParticleBehavior() = default;
};

// 重力挙動クラス
class GravityBehavior : public ParticleBehavior{
public:
    void Update(Particle& particle, float deltaTime) override;
};

// 振動挙動クラス
class SinusoidalBehavior : public ParticleBehavior{
public:
    void Update(Particle& particle, float deltaTime) override;
};
