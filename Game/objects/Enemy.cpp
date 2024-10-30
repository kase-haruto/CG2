#include "objects/Enemy.h" 
#include "Collision/CollisionManager.h"
#include "GameScene.h"

Enemy::Enemy() {
	//衝突判定のid設定
	Collider::SetTypeID(static_cast< uint32_t >(CollisionTypeIdDef::kEnemy));
	CollisionManager::GetInstance()->AddCollider(this);
} 

void Enemy::Initialize(Model* model){
	assert(!model);
	Character::Initialize(model_);
	model_->UpdateMatrix();

	
}

void Enemy::Initialize(Model* model, const Vector3& pos){
	model_ = model;
	model_->transform.translate = pos;
}

void Enemy::Update(){

	Character::Update();
}

void Enemy::Draw(){
	Character::Draw();
}

void Enemy::OnCollision(Collider* other){
	// 衝突相手の種別IDを取得
	uint32_t typeID = other->GetTypeID();
	if (typeID == static_cast< uint32_t >(CollisionTypeIdDef::kEnemy)){
		return;//早期リターン
	}

	//弾と当たったら死ぬ
	if (typeID == static_cast< uint32_t >(CollisionTypeIdDef::kBullet)){
		GameScene::newScore_ += score_;
		isAlive_ = false;
	}

	
}

const Vector3 Enemy::GetCenterPos() const{
	const Vector3 offset = {0.0f,1.5f,0.0f};
	Vector3 worldPos = Matrix4x4::Transform(offset, model_->worldMatrix);
	return worldPos;
}

void Enemy::Move(){
	if (true){

	}
}

