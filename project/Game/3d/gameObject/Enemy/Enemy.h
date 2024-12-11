#pragma once
#include "../BaseGameObject.h"
#include "../Engine/objects/Collider/SphereCollider.h"

class Enemy :
    public BaseGameObject,public SphereCollider{
public:
	//===================================================================*/
	//                   public func
	//===================================================================*/
	Enemy() = default;
	Enemy(const std::string& modelName);
	~Enemy()override;

	void Initialize()override;
	void Update()override;
	void Draw()override;
	void ImGui()override;

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
	Vector3 moveVelocity_ {};		// ˆÚ“®‘¬“x


private:
	/* Adjustment Variables =============================================*/
	float speed_ = 3.0f;

public:
	//===================================================================*/
	//                   getter
	//===================================================================*/
	const Transform& GetTransform()const{ return model_->transform; }
};