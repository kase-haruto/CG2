#pragma once
/* ========================================================================
/*	include space
/* ===================================================================== */
#include <Engine/Objects/3D/Actor/Actor.h>
#include <Game/3dObject/Actor/Bullet/Container/BulletContainer.h>
#include <Engine/Application/Effects/Particle/Emitter/FxEmitter.h>
#include <Engine/Renderer/Sprite/Sprite.h>

/* ========================================================================
/* Player
/* ===================================================================== */
class Player :
	public Actor {
	struct RollSet {
		bool isRolling_ = false;
		float rollTimer_ = 0.0f;
		float rollDuration_ = 0.5f;
		float rollStartAngle_ = 0.0f;
		float rollTargetAngle_ = 0.0f;
		float rollDirection_ = 1.0f; // +1 or -1：右回転 or 左回転
		Vector3 rollStartPos_{ 0.0f, 0.0f, 25.0f };
		Vector3 rollOffset_; // 例：奥に進む方向
	};

public:
	//===================================================================*/
	//                   public methods
	//===================================================================*/
	Player() = default;
	Player(const std::string& modelName,
		   std::optional<std::string> objectName = std::nullopt);
	virtual ~Player() = default;

	void Initialize()override;
	void Update()override;
	void Draw(ID3D12GraphicsCommandList* cmdList)override;

	/* ui =========================================*/
	void DerivativeGui()override;

	void SetParent(const WorldTransform* parent) {
		worldTransform_.parent = parent;
	}

	void SetBulletContainer(BulletContainer* bulletContainer) {
		bulletContainer_ = bulletContainer;
	}

private:
	//===================================================================*/
	//                   private methods
	//===================================================================*/
	void Move();
	void Shoot();
	void UpdateReticlePosition();
	void UpdateTilt(const Vector3& moveVector);
	void BarrelRoll();
	float EaseForwardThenReturn(float t);
	void InitializeEffect();

private:
	//===================================================================*/
	//                   private variables
	//===================================================================*/
	BulletContainer* bulletContainer_ = nullptr;	// 弾コンテナ
	float shootInterval_ = 0.3f;	// 発射間隔
	const float kMaxShootInterval_ = 0.3f;	// 最大発射間隔
	Vector3 lastMoveVector_;
	// ローリング関連
	RollSet rollSet_ = {};
	Vector3 reticleLocalOffset_ = Vector3(0.0f, 0.0f, 5.0f); // Playerからの相対位置（例：前方5m）
	WorldTransform reticleTransform_;
	std::vector < std::unique_ptr<Sprite>> lifeSprite_;
	std::unique_ptr<Sprite> attackSprite_;

	std::unique_ptr<FxEmitter> trailFx_;	// トレイルエフェクト
};
