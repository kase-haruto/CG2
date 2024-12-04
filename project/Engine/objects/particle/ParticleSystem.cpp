#include "ParticleSystem.h"

#include "ParticleManager.h"


ParticleSystem::ParticleSystem(){}

void ParticleSystem::Initialize(const std::string& modelName, const std::string& texturePath){

    BaseParticle::Initialize(modelName,texturePath, emitter_.count);

    ParticleManager::GetInstance()->AddSystem(this);

}