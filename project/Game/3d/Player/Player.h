#pragma once

// engine
#include "../Base/Character.h"
#include "../BaseGameObject.h"
#include "Engine/objects/Collider/BoxCollider.h"
#include "PlayerAttack/PlayerAttackController.h"
#include "PlayerAttack/PlayerAttackEditor.h"
#include "PlayerParticle/AttackParticle.h"
#include "Weapon/Weapon.h"

#include "PlayerParticle/DushParticle.h"
#include "Engine/physics/Triangle.h"

#include <optional>

	class PlayerState_Base;

	enum class PlayerState{
		Stay,
		Jog,
		Dush,
		Dead,
		Jump,
	};

class Player :
	public Character, public BoxCollider{
public:
	//===================================================================*/
	//                   public func
	//===================================================================*/
	Player() = default;
	Player(const std::string& modelName);
	~Player()override;

	void Initialize()override;
	void Update()override;
	void Draw()override;
	void ShowAttackEditorGui();
	void ShowGui()override;
	void TransitionState(PlayerState nextState);
	/* collision ======================================*/
	void OnCollisionEnter(Collider* other)override;
	void OnCollisionStay(Collider* other)override;
	void OnCollisionExit(Collider* other)override;

private:
	//===================================================================*/
	//                   private func
	//===================================================================*/


	std::string GetAttackInput(); // 攻撃名を取得する関数
private:
	//===================================================================*/
	//                   private variables
	//===================================================================*/
	bool isAttacking_ = false;									// 攻撃中フラグ
	std::unique_ptr<PlayerAttackController> attackController_;	// 攻撃管理クラス

	PlayerState state_ = PlayerState::Stay;		// プレイヤーの状態
	std::unique_ptr<PlayerState_Base> pState_;	// 状態クラス

	//武器
	std::unique_ptr<Weapon>weapon_ = nullptr;

	Vector3 forward_ = {0.0f,0.0f,1.0f};	// 前方ベクトル

	float jumpPower_ = 7.0f;	// ジャンプ力
	Vector3 jumpVelocity_ = {0.0f,0.0f,0.0f};	// ジャンプ速度	

	// プレイヤーのパーティクル
	std::unique_ptr<DushParticle> dushParticle_ = nullptr;
	std::unique_ptr<AttackParticle> attackParticle_ = nullptr;

public:
	PlayerAttackController* GetAttackController();
	Triangle triangle_;
public:
	//===================================================================*/
	//                   getter
	//===================================================================*/
	const EulerTransform& GetTransform() const;
	Vector3 GetForward() const;
	void SetForward(const Vector3& forward){ forward_ = forward; }
	Weapon* GetWeapon()const{ return weapon_.get(); }
	const Vector3 GetCenterPos()const override;
	Vector3& GetPosition()const{ return model_->transform.translate; }
	const Vector3& GetRotate()const{ return model_->transform.rotate; }
	const Matrix4x4 GetWorldMatrix()const;
	bool IsAttacking()const{ return attackController_->IsAttacking(); }
	void SetJumpVelocity(const Vector3& velocity){ jumpVelocity_ = velocity; }
	float GetJumpPower()const{ return jumpPower_; }
};
