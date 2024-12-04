#include "ParticleSystem.h"

#include "ParticleManager.h"


ParticleSystem::ParticleSystem() 
    : name_("DefaultParticleSystem"){

}

void ParticleSystem::Initialize(const std::string& modelName, const std::string& textureFilePath, uint32_t count){
    // 親クラス(BaseParticle)の初期化
    BaseParticle::Initialize(modelName, textureFilePath, count);

    // エミッターを初期化
    emitter_.Initialize(count);
}