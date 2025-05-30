#pragma once
#include <Engine/Objects/3D/Actor/Actor.h>
/* ========================================================================
/* include space
/* ===================================================================== */
#include <Engine/objects/Collider/SphereCollider.h>

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
	Enemy(const std::string& modelName);
	virtual ~Enemy() = default;

	void Initialize()override;
	void Update()override;

	void SetPosition(const Vector3& position){
		worldTransform_.translation = position;
	};

	void OnCollisionEnter(Collider* other)override;
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
};

