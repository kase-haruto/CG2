#include "IPlayerAttack.h"

#include "Engine/collision/CollisionManager.h"

#include <externals/imgui/imgui.h>

#include "Game/3d/gameObject/Player/Player.h"

IPlayerAttack::IPlayerAttack(){

	BoxCollider::Initialize(attackRange_.size);
	BoxCollider::colliderType_ = ColliderType::Type_PlayerAttack;
	BoxCollider::targetColliderType_ = ColliderType::Type_Enemy;
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

const Vector3 IPlayerAttack::GetPlayerPos() const{ return pPlayer_->GetCenterPos(); }