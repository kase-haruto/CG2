#pragma once
/* ========================================================================
/*	include space
/* ===================================================================== */
#include <Game/3dObject/Base/Actor.h>
#include <Game/3dObject/Actor/Bullet/Container/BulletContainer.h>
#include <Game/Effect/ParticleEffect/ParticleEffect.h>
/* ========================================================================
/* Player
/* ===================================================================== */
class Player :
	public Actor{
public:
	//===================================================================*/
	//                   public methods
	//===================================================================*/
	Player() = default;
	Player(const std::string& modelName,
		   std::function<void(IMeshRenderable*)> registerCB);
	virtual ~Player() = default;

	void Initialize()override;
	void Update()override;
	/* ui =========================================*/
	void DerivativeGui()override;

private:
	//===================================================================*/
	//                   private methods
	//===================================================================*/
	void Move();
	void Shoot();

private:
	//===================================================================*/
	//                   private variables
	//===================================================================*/
	std::unique_ptr<BulletContainer> bulletContainer_ = nullptr;	// 弾コンテナ
	float shootInterval_ = 0.3f;	// 発射間隔
	const float kMaxShootInterval_ = 0.3f;	// 最大発射間隔

	//efect
	std::unique_ptr<ParticleEffect> shootEffect_ = nullptr;	// パーティクルエフェクト
};

