#pragma once

// engine
#include "../BaseGameObject.h"
#include "Engine/objects/Collider/BoxCollider.h"
#include "Weapon.h"// local 
#include "Attack/HorizontalSlash.h"
#include "Engine/objects/particle/DemoParticle.h"

class Player :
    public BaseGameObject, public BoxCollider{
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
	void ImGui()override;
	void ShowDebugUI()override;

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
	std::unique_ptr<Weapon> weapon_ = nullptr;	// 武器


private:

	std::unique_ptr<HorizontalSlash> horizontalSlash_;
	bool isAttacking_ = false; // 攻撃中フラグ

	/* Adjustment Variables =============================================*/
	float speed_ = 3.0f;

public:
	//===================================================================*/
	//                   getter
	//===================================================================*/
	const Transform& GetTransform()const{ return model_->transform; }
	Vector3 GetForward(float distance) const;
};

