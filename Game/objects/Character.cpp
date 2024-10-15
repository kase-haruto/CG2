#include "objects/Character.h"

void Character::Initialize(Model* model){
	model_ = model;
}

void Character::Update(){
	//�s��̍X�V�Ȃ�
	model_->Update();
}

void Character::Draw(){
	//���f���`��
	model_->Draw();
}

void Character::SetViewProjection(const ViewProjection* viewProjection){
	if (!model_){
		return;
	}
	model_->SetViewProjection(viewProjection);
}
