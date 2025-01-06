#pragma once
#include "PlayerState_Base.h"

#include "lib/myMath/Vector3.h"

#include "../PlayerParticle/DushParticle.h"
#include "Engine/objects/particle/DemoParticle.h"

class PlayerState_Dush :
    public PlayerState_Base{
public:
	//===================================================================*/
	//                    main functions
	//===================================================================*/
	PlayerState_Dush() = default;
	PlayerState_Dush(Player* player);
	~PlayerState_Dush()override = default;

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
	float dushSpeed_ = 10.0f;
	float targetAngle_ = 0;						// 振り向き用
	Vector3 moveVelocity_ {};					// 移動速度
	std::unique_ptr<DemoParticle> dushParticle_ = nullptr;
};

