#pragma once
/* ========================================================================
/* include space
/* ===================================================================== */

#include <Game/3dObject/Actor/Bullet/BaseBullet.h>
#include <Engine/Objects/3D/Actor/SceneObject.h>

class SceneContext;

/* ========================================================================
/* bullet コンテナ
/* ===================================================================== */
class BulletContainer 
	: public SceneObject{
public:
	//===================================================================*/
	// public function
	//===================================================================*/
	BulletContainer(const std::string& name);
	~BulletContainer() = default;

	void Update();

	void AddBullet(const std::string& modelName,
				   const Vector3& position,
				   const Vector3& velocity);
	void RemoveBullet(BaseBullet* bullet);

	/* ui =========================================*/
	void ShowGui() override;
	virtual void DerivativeGui();

	/* config =========================================*/

	/* accessor =========================================*/
	const std::list<std::unique_ptr<BaseBullet>>& GetBullets() const{ return bullets_; }
	void SetSceneContext(SceneContext* context) { sceneContext_ = context; }
private:
	//===================================================================*/
	// private variables
	//===================================================================*/
	std::list<std::unique_ptr<BaseBullet>> bullets_; // 弾リスト
	SceneContext* sceneContext_ = nullptr;

	//===================================================================*/
	// adjustment variables
	//===================================================================*/
	float bulletSpeed_ = 60.0f; // 弾速
	Vector3 bulletScale_ {0.3f, 0.3f, 0.3f}; // 弾のスケール
};

