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
	
	Collider::type_ = ColliderType::Type_Enemy;
	Collider::targetType_ = 
		ColliderType::Type_Player |
		ColliderType::Type_PlayerAttack;

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
void Enemy::OnCollisionEnter([[maybe_unused]]Collider* other){
	
	//* 衝突相手がtargetType_に含まれていなければreturn
	if ((other->GetType() & Collider::GetTargetType()) != ColliderType::Type_None){

		//////////////////////////////////////////////////////////////////
		//				playerと衝突
		//////////////////////////////////////////////////////////////////
		if (other->GetType() == ColliderType::Type_Player){



		}

	}

}

void Enemy::OnCollisionStay([[maybe_unused]] Collider* other){}

void Enemy::OnCollisionExit([[maybe_unused]] Collider* other){}

/////////////////////////////////////////////////////////////////////////////////////
//					ui/ImGui
/////////////////////////////////////////////////////////////////////////////////////
void Enemy::ShowGui(){

#ifdef _DEBUG
	SceneObject::ShowGui();

	if (ImGui::Button("save")){
		JsonCoordinator::SaveGroup(BaseGameObject::GetName(), BaseGameObject::jsonPath);
	}

	ImGui::Separator();

	BaseGameObject::ShowGui();

	ImGui::Separator();

	BoxCollider::ShowGui();
#endif // _DEBUG

}