#include "IPlayerAttack.h"
#include "Engine/collision/CollisionManager.h"
#include "Engine/core/Json/JsonCoordinator.h"

#include "../Player.h"

#include <externals/imgui/imgui.h>


IPlayerAttack::IPlayerAttack(const std::string& attackName){

	//colliderの設定
	CollisionManager::GetInstance()->AddCollider(this);
	Collider::SetName(attackName);

	Collider::type_ = ColliderType::Type_PlayerAttack;
	Collider::targetType_ = ColliderType::Type_Enemy;

	BoxCollider::Initialize(Vector3(1.0f, 1.0f, 1.0f));

	rotate_ = Vector3(0.0f, 0.0f, 0.0f);
	offset_ = 0.0f;

	std::string directory = "gameObjects/Player";
	JsonCoordinator::LoadGroup(Collider::GetName().c_str(), directory);


}

IPlayerAttack::~IPlayerAttack(){
	CollisionManager::GetInstance()->RemoveCollider(this);
}

void IPlayerAttack::SetPlayer(const Player* pPlayer){
	pPlayer_ = pPlayer;
}
