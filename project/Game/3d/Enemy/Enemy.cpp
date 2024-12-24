#include "Enemy.h"

#include "Engine/collision/CollisionManager.h"
#include "Engine/core/Json/JsonCoordinator.h"
#include <externals/imgui/imgui.h>

Enemy::Enemy(const std::string& modelName)
:Character(modelName){
	CollisionManager::GetInstance()->AddCollider(this);
}

Enemy::~Enemy(){
	CollisionManager::GetInstance()->RemoveCollider(this);
}


/////////////////////////////////////////////////////////////////////////////////////
//					mainFunction
/////////////////////////////////////////////////////////////////////////////////////
void Enemy::Initialize(){

	BaseGameObject::SetName("Enemy");
	Collider::SetName("Enemy");
	
	//コライダー
	BaseGameObject::Initialize();
	BoxCollider::Initialize(model_->transform.scale);
	
	moveSpeed_ = 2.0f;
}

void Enemy::Update(){

	//衝突処理の更新
	shape_.center = GetCenterPos();
	shape_.rotate = model_->transform.rotate;

	//モデルの
	BaseGameObject::Update();
	//落下処理
	Character::Update();
}

void Enemy::Draw(){
	BoxCollider::Draw();
	BaseGameObject::Draw();
}


/////////////////////////////////////////////////////////////////////////////////////
//					Collision
/////////////////////////////////////////////////////////////////////////////////////
void Enemy::OnCollisionEnter([[maybe_unused]]Collider* other){}

void Enemy::OnCollisionStay([[maybe_unused]] Collider* other){}

void Enemy::OnCollisionExit([[maybe_unused]] Collider* other){}

/////////////////////////////////////////////////////////////////////////////////////
//					ui/ImGui
/////////////////////////////////////////////////////////////////////////////////////
void Enemy::ShowGui(){
#ifdef _DEBUG
	if (ImGui::Button("save")){
		JsonCoordinator::SaveGroup(BaseGameObject::GetName());
	}

	ImGui::Separator();

	BaseGameObject::ShowGui();

	ImGui::Separator();

	BoxCollider::ShowGui();
#endif // _DEBUG

}