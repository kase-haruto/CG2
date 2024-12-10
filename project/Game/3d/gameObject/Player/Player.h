#pragma once

// engine
#include "Game/3d/BaseGameObject.h"
#include "Engine/objects/Collider/BoxCollider.h"

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
	float speed_ = 3.0f;
	float targetAngle_ = 0;
	Vector3 moveVelocity_ {};


public:
	//===================================================================*/
	//                   getter
	//===================================================================*/
	const Transform& GetTransform()const{ return model_->transform; }
};

