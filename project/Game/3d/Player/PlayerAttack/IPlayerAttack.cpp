#include "IPlayerAttack.h"
#include "Engine/collision/CollisionManager.h"
#include "Engine/core/Json/JsonCoordinator.h"

#include "../Player.h"

#include <externals/imgui/imgui.h>


IPlayerAttack::IPlayerAttack(const std::string& attackName){

	//colliderの設定
	Collider::SetName(attackName);

	rotate_ = Vector3(0.0f, 0.0f, 0.0f);
	offset_ = 0.0f;


}

IPlayerAttack::~IPlayerAttack(){
	CollisionManager::GetInstance()->RemoveCollider(this);
}

void IPlayerAttack::Initialize(){
	CollisionManager::GetInstance()->AddCollider(this);

	Collider::type_ = ColliderType::Type_PlayerAttack;
	Collider::targetType_ = ColliderType::Type_Enemy;

	BoxCollider::Initialize(Vector3(1.0f, 1.0f, 1.0f));

	std::string directory = "gameObjects/Player";
	JsonCoordinator::LoadGroup(Collider::GetName().c_str(), directory);

	isAttacking_ = true;
	isActive_ = true;
}

IPlayerAttack::IPlayerAttack(const IPlayerAttack& other)
	: BoxCollider(other), // 基底クラス BoxCollider のコピー
	center_(other.center_),
	offset_(other.offset_),
	rotate_(other.rotate_),
	isAttacking_(other.isAttacking_),
	weapon_(other.weapon_),     // シャローコピー: 元のポインタをそのままコピー
	pPlayer_(other.pPlayer_),   // シャローコピー
	damage_(other.damage_),
	attackName_(other.attackName_){
}

void IPlayerAttack::SetPlayer(Player* pPlayer){
	pPlayer_ = pPlayer;
}

const Vector3 IPlayerAttack::GetPlayerPos() const{
	return pPlayer_->GetCenterPos();
}
