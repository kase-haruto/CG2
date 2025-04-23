#include "PlayerBullet.h"

PlayerBullet::PlayerBullet(const std::string& modelName,
						   std::function<void(IMeshRenderable*)> registerCB)
:BaseBullet::BaseBullet(modelName,registerCB){

}

