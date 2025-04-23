#pragma once
/* ========================================================================
/* include space
/* ===================================================================== */
#include <Game/3dObject/Base/Actor.h>

/* ========================================================================
/* bullet 基底クラス
/* ===================================================================== */
class BaseBullet :
	public Actor{
public:
	//===================================================================*/
	//			public function
	//===================================================================*/
	BaseBullet() = default;
	BaseBullet(const std::string& modelName,
			   std::function<void(IMeshRenderable*)> registerCB);
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

