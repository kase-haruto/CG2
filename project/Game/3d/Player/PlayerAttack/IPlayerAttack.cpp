#include "IPlayerAttack.h"
#include "Engine/collision/CollisionManager.h"
#include "Engine/core/Json/JsonCoordinator.h"

#include <externals/imgui/imgui.h>


IPlayerAttack::IPlayerAttack(const std::string& attackName){

	//colliderの設定
	CollisionManager::GetInstance()->AddCollider(this);
	Collider::SetName(attackName);

	Collider::type_ = ColliderType::Type_PlayerAttack;
	Collider::targetType_ = ColliderType::Type_Enemy;

	BoxCollider::Initialize(Vector3(1.0f, 1.0f, 1.0f));

	rotate_ = Vector3(0.0f, 0.0f, 0.0f);
	offset_ = Vector3(0.0f, 0.0f, 0.0f);

	JsonCoordinator::LoadGroup(Collider::GetName().c_str());

}

IPlayerAttack::~IPlayerAttack(){
	CollisionManager::GetInstance()->RemoveCollider(this);
}

void IPlayerAttack::Execution(){
	Initialize();
	isAttacking_ = true;
}

void IPlayerAttack::Update(){
	shape_.center = center_;
	shape_.rotate = rotate_;
}

void IPlayerAttack::Draw(){
	if (isAttacking_){
		BoxCollider::Draw();
	}
}

void IPlayerAttack::ShowGui(){

	ImGui::Text(Collider::GetName().c_str());

	if (ImGui::Button("save")){
		JsonCoordinator::SaveGroup(Collider::GetName().c_str());
	}

	ImGui::Separator();

	ImGui::Text("center:x = %.1f,y = %.1f,z = %.1f",center_.x,center_.y,center_.z);
	ImGui::Text("rotate:x = %.1f,y = %.1f,z = %.1f", rotate_.x, rotate_.y,rotate_.z);
	ImGui::DragFloat3("offset", &offset_.x, 0.01f);

	BoxCollider::ShowGui();

}
