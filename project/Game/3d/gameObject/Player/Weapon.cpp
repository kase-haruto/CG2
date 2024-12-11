#include "Weapon.h"

#include "Engine/Collision/CollisionManager.h"

#include <externals/imgui/imgui.h>

#include "lib/myFunc/MyFunc.h"

Weapon::Weapon(const std::string& modelName)
:BaseGameObject(modelName){

	// 細長くする
	BoxCollider::Initialize(model_->transform.scale);
	CollisionManager::GetInstance()->AddCollider(this);

}

Weapon::~Weapon(){}

///////////////////////////////////////////////////////////////////////////////
//					main method
///////////////////////////////////////////////////////////////////////////////
void Weapon::Initialize(){

	BaseGameObject::SetName("playerWeapon");
	Collider::SetName("playerWeapon");
	BaseGameObject::Initialize();
}

void Weapon::Update(){

	//攻撃をしていないときは判定をつけない
	if (!isAttacking_){
		shape_.size = {0.0f,0.0f,0.0f};
	}

	shape_.center = GetCenterPos();
	shape_.rotate = model_->transform.rotate;

	BaseGameObject::Update(true);

}

void Weapon::Draw(){

	BoxCollider::Draw();
	BaseGameObject::Draw();

}

void Weapon::UpdateWorldMat(){


	model_->worldMatrix = MakeAffineMatrix(model_->transform.scale, model_->transform.rotate, model_->transform.translate);

}
///////////////////////////////////////////////////////////////////////////////
//					ui/imgui
///////////////////////////////////////////////////////////////////////////////
void Weapon::ImGui(){

#ifdef _DEBUG
	BaseGameObject::ImGui();

	ImGui::DragFloat3("colliderSize",&shape_.size.x,0.01f);
#endif // _DEBUG

}


///////////////////////////////////////////////////////////////////////////////
//					collision
///////////////////////////////////////////////////////////////////////////////

void Weapon::OnCollisionEnter([[maybe_unused]]Collider* other){}

void Weapon::OnCollisionStay([[maybe_unused]] Collider* other){}

void Weapon::OnCollisionExit([[maybe_unused]] Collider* other){}

void Weapon::SetCollisionSpace(const Vector3& position, const Vector3& size){
	isAttacking_ = true;
	shape_.center = position;
	shape_.size = size;
}
