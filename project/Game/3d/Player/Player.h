#pragma once

// engine
#include "../Base/Character.h"
#include "../BaseGameObject.h"
#include "Engine/objects/Collider/BoxCollider.h"
#include "PlayerAttack/PlayerAttackController.h"
#include "PlayerAttack/PlayerAttackEditor.h"
#include "Weapon/Weapon.h"

#include <optional>

	class PlayerState_Base;

	enum class PlayerState{
		Stay,
		Jog,
		Dush,
		Dead,
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

	/* collision ======================================*/
	void OnCollisionEnter(Collider* other)override;
	void OnCollisionStay(Collider* other)override;
	void OnCollisionExit(Collider* other)override;

private:
	//===================================================================*/
	//                   private func
	//===================================================================*/
	void TransitionState(PlayerState nextState);

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

public:
	PlayerAttackController* GetAttackController();

public:
	//===================================================================*/
	//                   getter
	//===================================================================*/
	const EulerTransform& GetTransform() const;
	Vector3 GetForward(float distance) const;
	Weapon* GetWeapon()const{ return weapon_.get(); }
	const Vector3 GetCenterPos()const override;
	const Matrix4x4 GetWorldMatrix()const;
	bool IsAttacking()const{ return attackController_->IsAttacking(); }
};
