#include "PlayerBullet.h"

PlayerBullet::PlayerBullet(const std::string& modelName,
						   std::function<void(IMeshRenderable*, const WorldTransform*)> registerCB)
:BaseBullet::BaseBullet(modelName,registerCB){

}

