#include "ParticleSystem.h"

#include "ParticleManager.h"


ParticleSystem::ParticleSystem(){}

void ParticleSystem::Initialize(const std::string& modelName){

    BaseParticle::Initialize(modelName, emitter_.count);

    ParticleManager::GetInstance()->AddSystem(this);

}