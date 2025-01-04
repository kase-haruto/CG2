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


public:
	PlayerAttackController* GetAttackController();

public:
	//===================================================================*/
	//                   getter
	//===================================================================*/
	const EulerTransform& GetTransform() const;
	Vector3 GetForward() const;
	void SetForward(const Vector3& forward){ forward_ = forward; }
	Weapon* GetWeapon()const{ return weapon_.get(); }
	const Vector3 GetCenterPos()const override;
	const Vector3& GetRotate()const{ return model_->transform.rotate; }
	const Matrix4x4 GetWorldMatrix()const;
	bool IsAttacking()const{ return attackController_->IsAttacking(); }
};
