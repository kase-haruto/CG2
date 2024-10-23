#include "objects/Bullet.h"



Bullet::~Bullet(){
	delete model_;
}

void Bullet::Initialize(Model* model){
	model_ = model;
}

void Bullet::Initialize(Model* model, const Vector3& pos,const Vector3& vel){
	model_ = model;

	model_->transform.translate = pos;
	model_->UpdateMatrix();

	velocity_ = vel;
}

void Bullet::Update(){

	//一定時間経過でデス
	if (--deathTimer_<=0){
		isActive_ = false;
	}
	
	//座標の更新
	model_->transform.translate += velocity_;

	//行列などの更新
	model_->Update();
}

void Bullet::Draw(){
	if (isActive_){
		model_->Draw();
	}
}

void Bullet::SetViewProjection(const ViewProjection* viewProjection){
	if (!model_){
		return;
	}
	model_->SetViewProjection(viewProjection);
}
