#pragma once
#include "../BaseBullet.h"

class PlayerBullet :
    public BaseBullet{
public:
	//===================================================================*/
	//			public function
	//===================================================================*/
	PlayerBullet() = default;
	PlayerBullet(const std::string& modelName);
	~PlayerBullet()override = default;

	//--------- accessor ---------------------------------------------------
};

