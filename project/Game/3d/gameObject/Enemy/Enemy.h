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
	void UpdateVelocities();

private:
	//===================================================================*/
	//                   private variables
	//===================================================================*/
	Vector3 moveVelocity_ {};					// 移動速度
	Vector3 knockbackVelocity_ {};				// のけぞり速度
	Vector3	gravityVelocity_ {};				// 重力
	Vector3 totalVelocity_ {};					// すべての速度のトータル

	Vector3 acceleration_ {0.0f,-9.8f,0.0f};	// 加速度

	bool isGrounded_ = true;

private:
	/* Adjustment Variables =============================================*/
	float speed_ = 3.0f;

public:
	//===================================================================*/
	//                   getter
	//===================================================================*/
	const Transform& GetTransform()const{ return model_->transform; }
	const Vector3 GetCenterPos()const;
};