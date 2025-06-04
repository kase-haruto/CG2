#pragma once
#include "../BaseBullet.h"

class PlayerBullet :
    public BaseBullet{
public:
	//===================================================================*/
	//			public function
	//===================================================================*/
	PlayerBullet() = default;
	PlayerBullet(const std::string& modelName,const std::string& name);
	~PlayerBullet()override = default;

	void Update() override;
	//--------- accessor ---------------------------------------------------
};

