#include "objects/Character.h"

void Character::Initialize(Model* model){
	model_ = model;
}

void Character::Update(){
	//行列の更新など
	model_->Update();
}

void Character::Draw(){
	//モデル描画
	model_->Draw();
}

void Character::SetViewProjection(const ViewProjection* viewProjection){
	if (!model_){
		return;
	}
	pViewProjection_ = viewProjection;

	model_->SetViewProjection(viewProjection);
}
