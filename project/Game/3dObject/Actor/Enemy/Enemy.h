#pragma once
#include <Game/3dObject/Base/Actor.h>
/* ========================================================================
/* include space
/* ===================================================================== */
#include <Engine/objects/Collider/SphereCollider.h>

/* ========================================================================
/* enemy
/* ===================================================================== */
class Enemy :
    public Actor,SphereCollider{
public:
	//===================================================================*/
	//					public methods
	//===================================================================*/
	Enemy() = default;
	Enemy(const std::string& modelName,
		  std::function<void(IMeshRenderable*)> registerCB);
	virtual ~Enemy() = default;

	void Initialize()override;
	void Update()override;
	void Draw()override;

	void SetPosition(const Vector3& position){
		model_->worldTransform_.translation = position;
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

