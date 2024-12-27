#pragma once

// engine
#include "../Base/Character.h"
#include "../BaseGameObject.h"
#include "Engine/objects/Collider/BoxCollider.h"
#include "PlayerAttack/PlayerAttackController.h"

class Player :
	public Character, public BoxCollider{
public:
	//===================================================================*/
	//                   public func
	//===================================================================*/
	Player() = default;
	Player(const std::string& modelName);
	~Player()override;

	void Initialize()override;
	void Update()override;
	void Draw()override;
	void ShowGui()override;

	/* collision ======================================*/
	void OnCollisionEnter(Collider* other)override;
	void OnCollisionStay(Collider* other)override;
	void OnCollisionExit(Collider* other)override;

private:
	//===================================================================*/
	//                   private func
	//===================================================================*/
	void Move();

private:
	//===================================================================*/
	//                   private variables
	//===================================================================*/
	float targetAngle_ = 0;						// 振り向き用
	Vector3 moveVelocity_ {};					// 移動速度

	bool isAttacking_ = false;					// 攻撃中フラグ
	PlayerAttackController attackController_;	// 攻撃管理クラス


public:
	//===================================================================*/
	//                   getter
	//===================================================================*/
	const EulerTransform& GetTransform()const{ return model_->transform; }
	Vector3 GetForward(float distance) const;

	const Vector3 GetCenterPos()const override;
};
