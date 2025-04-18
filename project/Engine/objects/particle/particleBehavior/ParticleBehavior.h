#pragma once

#include <externals/nlohmann/json.hpp>
class Particle;

class ParticleBehavior{
public:
	ParticleBehavior() = default;
    virtual ~ParticleBehavior() = default;
    virtual void ApplyBehavior(Particle& particle) = 0;

    virtual void ShowImGui(){};
    virtual void SaveJson([[maybe_unused]]nlohmann::json& j){};
    virtual void LoadJson([[maybe_unused]]nlohmann::json& j){};
};

