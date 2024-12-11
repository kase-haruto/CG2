#include "Enemy.h"


#include "Engine/Collision/CollisionManager.h"

Enemy::Enemy(const std::string& modelName)
:BaseGameObject(modelName){

	SphereCollider::Initialize(model_->transform.scale.x * 0.5f + 0.1f);
	
	CollisionManager::GetInstance()->AddCollider(this);
}

Enemy::~Enemy(){

	CollisionManager::GetInstance()->RemoveCollider(this);

}

/////////////////////////////////////////////////////////////////////////
//						main fanc
/////////////////////////////////////////////////////////////////////////
void Enemy::Initialize(){

	BaseGameObject::SetName("enemy");
	SphereCollider::SetName("enemy");
	BaseGameObject::Initialize();

}

void Enemy::Update(){

	shape_.center = model_->transform.translate;

	BaseGameObject::Update();

}

void Enemy::Draw(){

	//SphereCollider::Draw();
	BaseGameObject::Draw();

}

void Enemy::Move(){}

/////////////////////////////////////////////////////////////////////////
//						imgui / ui
/////////////////////////////////////////////////////////////////////////
void Enemy::ImGui(){}

/////////////////////////////////////////////////////////////////////////
//						collision
/////////////////////////////////////////////////////////////////////////
void Enemy::OnCollisionEnter([[maybe_unused]] Collider* other){}

void Enemy::OnCollisionStay([[maybe_unused]] Collider* other){}

void Enemy::OnCollisionExit([[maybe_unused]] Collider* other){}

