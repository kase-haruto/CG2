#pragma once
#include "PlayerState_Base.h"
#include "lib/myMath/Vector3.h"

class PlayerState_Jump 
	: public PlayerState_Base {
	public:
		//===================================================================*/
		//                    main functions
		//===================================================================*/
		PlayerState_Jump() = default;
		PlayerState_Jump(Player* player);
		~PlayerState_Jump()override = default;

		void Initialize()override;
		void Update()override;
		void Draw()override;
		void Cleanup()override;

		void ShowGui()override;

	private:
		//===================================================================*/
		//                    private functions
		//===================================================================*/
		void Jump();
		void Move();

	private:
		//===================================================================*/
		//                    private variables
		//===================================================================*/
		float animationSpeed_ = 1.3f;
		float jumpPower = 5.0f;
		float moveSpeed_ = 3.0f;
		bool hasJumped_ = false;

		float targetAngle_ = 0;						// 振り向き用
		Vector3 moveVelocity_ {};					// 移動速度
};

