#pragma once

//engine
#include "Engine/objects/Collider/BoxCollider.h"
#include "Engine/objects/particle/DemoParticle.h"
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
	AttackRange attackRange_;		// 技の範囲と座標(武器のコライダーに渡す)
	bool isAttacking_ = false;		// 

	Player* pPlayer_ = nullptr;
	std::unique_ptr<DemoParticle> particle_ = nullptr;
public:
	//===================================================================*/
	//                    getter / setter
	//===================================================================*/
	void SetPlayer(Player* player){ pPlayer_ = player; }
	bool GetIsAttacking()const{ return isAttacking_; }
	const Vector3& GetRangePos()const{ return attackRange_.position; }
};