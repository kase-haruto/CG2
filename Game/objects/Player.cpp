#include "objects/Player.h"
#include "myFunc/MyFunc.h"
void Player::Initialize(Model* model){
	Character::Initialize(model);
}

void Player::Update(){

	

	//親がいたらそれも計算
	if (parentTransform_){
		parentWorldMat_ = MakeAffineMatrix(parentTransform_->scale,
										   parentTransform_->rotate,
										   parentTransform_->translate);
		model_->worldMatrix = Matrix4x4::Multiply(model_->worldMatrix, parentWorldMat_);
	}
	//行列の更新
	Character::Update();
}

void Player::Draw(){
	Character::Draw();
}
