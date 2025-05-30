#pragma once
#include <Engine/Objects/3D/Actor/Actor.h>
/* ========================================================================
/* include space
/* ===================================================================== */
#include <Engine/objects/Collider/SphereCollider.h>
#include <Game/Effect/ParticleEffect/ParticleEffect.h>
/* ========================================================================
/* enemy
/* ===================================================================== */
class Enemy :
    public Actor{
public:
	//===================================================================*/
	//					public methods
	//===================================================================*/
	Enemy() = default;
	Enemy(const std::string& modelName,const std::string objName);

	virtual ~Enemy() = default;

	void InitializeEffect();
	void Initialize()override;
	void Update()override;


	void OnCollisionEnter(Collider* other)override;
	void OnCollisionStay([[maybe_unused]]Collider* other)override {};
	void OnCollisionExit([[maybe_unused]] Collider* other)override {};
	const Vector3 GetCenterPos()const override;
	void SetPosition(const Vector3& position){
		worldTransform_.translation = position;
	};

private:
	//===================================================================*/
	//					private methods
	//===================================================================*/
	void Move();
	void Shoot();

private:
	//===================================================================*/
	//					private variables
	//===================================================================*/
	bool isHit_ = false;		// 衝突フラグ
	ParticleEffect* hitEffect_ = nullptr;

};

