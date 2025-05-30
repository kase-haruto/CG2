#include "PlayerBullet.h"
#include <Game/Effect/ParticleEffect/ParticleEffectCollection.h>

PlayerBullet::PlayerBullet(const std::string& modelName, const std::string& name)
:BaseBullet::BaseBullet(modelName, name){

}

void PlayerBullet::Update(){

	BaseBullet::Update();
}

