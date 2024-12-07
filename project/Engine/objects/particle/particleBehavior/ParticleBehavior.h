#pragma once

#include <nlohmann/json.hpp>
class ParticleSystem;

class ParticleBehavior{
public:
    virtual ~ParticleBehavior() = default;
    virtual void ApplyBehavior(ParticleSystem& particle) = 0;

    virtual void ShowImGui(){};
    virtual void SaveJson([[maybe_unused]]nlohmann::json& j){};
    virtual void LoadJson([[maybe_unused]]nlohmann::json& j){};
};

