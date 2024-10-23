#include "objects/Player.h"
#include "myFunc/MyFunc.h"
#include "myFunc/MathFunc.h"
#include "core/Input.h"

void Player::Initialize(Model* model){
	Character::Initialize(model);
}

void Player::Update(){
	///==============================
	//古い球の削除
	bullets_.remove_if([] (const std::unique_ptr<Bullet>& bullet){
		if (!bullet->GetIsActive()){
			return true;
		}
		return false;
					   });

	//===============================

	Shoot();

	// 弾の更新
	for (auto& bullet : bullets_){
		bullet->Update();
	}


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
	for (auto& bullet : bullets_){
		bullet->Draw();
	}

	if (!isRail_){
		Character::Draw();
	}
}

void Player::Shoot(){
	if (Input::TriggerKey(DIK_SPACE)){
		//弾の速度
		const float kBulletSpeed = 1.0f;
		Vector3 bulletVal = {0.0f,0.0f,kBulletSpeed};

		// ワールド行列から前方ベクトルを取得するラムダ式
		auto getForwardVector = [] (const Matrix4x4& worldMatrix) -> Vector3{
			return Vector3 {
				worldMatrix.m[2][0], // 3列目（Z軸）のX成分
				worldMatrix.m[2][1], // 3列目（Z軸）のY成分
				worldMatrix.m[2][2]  // 3列目（Z軸）のZ成分
			}.Normalize();
			};

		//前方に弾を飛ばす
		bulletVal = getForwardVector(model_->worldMatrix).Normalize();

		// 弾を生成してユニークポインタにラップ
		Model* model = new Model("debugCube");
		std::unique_ptr<Bullet> newBullet = std::make_unique<Bullet>();
		model->SetViewProjection(pViewProjection_);

		newBullet->Initialize(std::move(model), this->GetWorldPosition(), bulletVal);


		// 弾を登録
		bullets_.push_back(std::move(newBullet));
	}
}
