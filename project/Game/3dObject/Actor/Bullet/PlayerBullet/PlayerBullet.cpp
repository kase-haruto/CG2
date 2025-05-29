#include "PlayerBullet.h"
#include <Game/Effect/ParticleEffect/ParticleEffectCollection.h>

PlayerBullet::PlayerBullet(const std::string& modelName)
:BaseBullet::BaseBullet(modelName){

}

void PlayerBullet::Update(){

	BaseBullet::Update();
}

