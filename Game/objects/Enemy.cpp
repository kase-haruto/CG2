#include "objects/Enemy.h" 


Enemy::Enemy() {} 



void Enemy::Initialize(Model* model){
	Character::Initialize(model_);
	model_->UpdateMatrix();
	assert(!model_);
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

void Enemy::Move(){

}

