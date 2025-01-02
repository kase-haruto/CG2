#pragma once

#include "PlayerState_Base.h"
#include "lib/myMath/Vector3.h"

class PlayerState_Jog
:public PlayerState_Base{
public:
	//===================================================================*/
	//                    main functions
	//===================================================================*/
	PlayerState_Jog() = default;
	PlayerState_Jog(Player* player);
	~PlayerState_Jog()override = default;
	void Initialize()override;
	void Update()override;
	void Draw()override;
	void Cleanup()override;

	void ShowGui()override;

private:
	//===================================================================*/
	//                    private functions
	//===================================================================*/
	void Move();

private:
	//===================================================================*/
	//                    private variables
	//===================================================================*/
	float animationSpeed_ = 1.3f;
	float jogSpeed_ = 6.0f;

	float targetAngle_ = 0;						// 振り向き用
	Vector3 moveVelocity_ {};					// 移動速度
};

