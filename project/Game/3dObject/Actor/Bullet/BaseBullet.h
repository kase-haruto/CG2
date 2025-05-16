#pragma once
/* ========================================================================
/* include space
/* ===================================================================== */
#include <Engine/Objects/3D/Actor/Actor.h>
#include <Engine/objects/Collider/SphereCollider.h>

/* ========================================================================
/* bullet 基底クラス
/* ===================================================================== */
class BaseBullet :
	public Actor,SphereCollider{
public:
	//===================================================================*/
	//			public function
	//===================================================================*/
	BaseBullet() = default;
	BaseBullet(const std::string& modelName);
	virtual ~BaseBullet() = default;

	virtual void Initialize(const Vector3 initPos,const Vector3 velocity);
	void Update()override;
	void DerivativeGui()override;

	//--------- accessor ---------------------------------------------------

	//===================================================================*/
	//private methods
	//===================================================================*/

protected:
	//===================================================================*/
	//			protected variables
	//===================================================================*/
	float lifeTime_ = 3.0f;      // 弾の寿命（秒）
	float currentTime_ = 0.0f;   // 経過時間
};

