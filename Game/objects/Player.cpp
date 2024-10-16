#include "objects/Player.h"
#include "myFunc/MyFunc.h"
#include "myFunc/MathFunc.h"

void Player::Initialize(Model* model){
	Character::Initialize(model);
}

void Player::Update(){



	//行列の更新
	model_->worldMatrix = MakeAffineMatrix(model_->transform.scale,
										   model_->transform.rotate,
										   model_->transform.translate);

	//親がいたらそれも計算
	if (parentTransform_){
		parentWorldMat_ = MakeAffineMatrix({1.0f,1.0f,1.0f},
										   parentTransform_->rotate,
										   parentTransform_->translate);
		model_->worldMatrix = Matrix4x4::Multiply(parentWorldMat_, model_->worldMatrix);
	}
	model_->UpdateMatrix();

}

void Player::Draw(){
	if (!isRail_){
		Character::Draw();
	}
}
