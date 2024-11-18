#include "engine/objects/ParticleSystem/ParticleBehavior.h"
#include <cmath>

// 重力挙動の実装
void GravityBehavior::Update(Particle& particle, float deltaTime){
    float velocityY = particle.attributes["velocityY"];
    velocityY -= 9.8f * deltaTime; // 重力加速度
    particle.attributes["velocityY"] = velocityY;
}

// 振動挙動の実装
void SinusoidalBehavior::Update(Particle& particle, float deltaTime){
    float time = particle.attributes["currentTime"];
    float offsetY = std::sin(time);
    particle.attributes["translateY"] += offsetY * deltaTime;
}
