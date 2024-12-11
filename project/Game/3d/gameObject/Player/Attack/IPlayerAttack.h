#pragma once

//engine
#include "Engine/objects/Collider/BoxCollider.h"
// lib
#include "lib/myMath/Vector3.h"

struct AttackRange{
	Vector3 position;
	Vector3 size {1.0f,1.0f,1.0f};
};

class Player;
class Weapon;

class IPlayerAttack
	:public BoxCollider{

public:
	//===================================================================*/
	//                    public methods
	//===================================================================*/
	IPlayerAttack();
	virtual ~IPlayerAttack();
	virtual void Initialize(){};
	virtual void Execution() = 0;
	virtual void Draw();

protected:
	//===================================================================*/
	//                    protected methods
	//===================================================================*/
	float damage_;					// 技が与えるダメージ
	float force_;					// 吹き飛ばしの力の強さ
	AttackRange attackRange_;		// 技の範囲と座標(武器のコライダーに渡す)
	bool isAttacking_ = false;		// 攻撃しているか


	Player* pPlayer_ = nullptr;

public:
	//===================================================================*/
	//                    getter / setter
	//===================================================================*/
	void SetPlayer(Player* player){ pPlayer_ = player; }
	const Vector3 GetPlayerPos()const;
	bool GetIsAttacking()const{ return isAttacking_; }
	float GetForce()const{ return force_; }
	const Vector3& GetRangePos()const{ return attackRange_.position; }
};