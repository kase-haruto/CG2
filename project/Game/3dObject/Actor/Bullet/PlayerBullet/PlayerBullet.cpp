#include "PlayerBullet.h"
#include <Game/Effect/ParticleEffect/ParticleEffectCollection.h>

PlayerBullet::PlayerBullet(const std::string& modelName, const std::string& name)
:BaseBullet::BaseBullet(modelName, name){
	collider_->SetType(ColliderType::Type_PlayerAttack);
	collider_->SetTargetType(ColliderType::Type_Enemy);

}

void PlayerBullet::Update(){

	BaseBullet::Update();
}

