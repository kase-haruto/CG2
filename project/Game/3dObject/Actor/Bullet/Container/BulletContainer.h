#pragma once
/* ========================================================================
/* include space
/* ===================================================================== */

#include <Game/3dObject/Actor/Bullet/BaseBullet.h>
#include <Engine/objects/SceneObject.h>

/* ========================================================================
/* bullet コンテナ
/* ===================================================================== */
class BulletContainer :
	public SceneObject{
public:
	//===================================================================*/
	//			public function
	//===================================================================*/
	BulletContainer(const std::string& name, std::function<void(IMeshRenderable*)> registerCB);
	~BulletContainer() = default;

	void Update();

	void AddBullet(const std::string& modelName,
				   const Vector3& position,
				   const Vector3& velocity);
	void RemoveBullet(BaseBullet* bullet);
	/* ui =========================================*/
	void ShowGui()override;
	virtual void DerivativeGui(){}

	/* accessor =========================================*/
	const std::list<std::unique_ptr<BaseBullet>>& GetBullets() const{ return bullets_; }

private:
	//===================================================================*/
	//			private variables
	//===================================================================*/
	std::list<std::unique_ptr<BaseBullet>> bullets_;	// 弾リスト
	std::function<void(IMeshRenderable*)> registerCB_ = nullptr;	// モデル登録コールバック

private:
	//===================================================================*/
	//			adjustment variables
	//===================================================================*/
	float bulletSpeed_ = 40.0f;	// 弾速
	Vector3 bulletScale_ {0.2f,0.2f,1.5f};		// 弾のスケール
};

