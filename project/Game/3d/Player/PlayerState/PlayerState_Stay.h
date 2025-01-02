#pragma once
#include "PlayerState_Base.h"

class PlayerState_Stay
:public PlayerState_Base{
public:
	//===================================================================*/
	//                    main functions
	//===================================================================*/
	PlayerState_Stay() = default;
	PlayerState_Stay(Player* player);
	~PlayerState_Stay()override = default;

	void Initialize()override;
	void Update()override;
	void Draw()override;
	void Cleanup()override;

	void ShowGui()override;


private:
	//===================================================================*/
	//                    private variables
	//===================================================================*/
	float animationSpeed_ = 1.0f;

};

