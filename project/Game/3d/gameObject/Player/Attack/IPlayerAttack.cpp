#include "IPlayerAttack.h"

#include "Engine/collision/CollisionManager.h"

#include <externals/imgui/imgui.h>

IPlayerAttack::IPlayerAttack(){

	BoxCollider::Initialize(attackRange_.size);
	CollisionManager::GetInstance()->AddCollider(this);

}

IPlayerAttack::~IPlayerAttack(){

	CollisionManager::GetInstance()->RemoveCollider(this);

}


void IPlayerAttack::Draw(){

	if (isAttacking_){
		BoxCollider::Draw();
	}

}
