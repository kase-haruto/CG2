#include "objects/Player.h"
#include "myFunc/MyFunc.h"
#include "myFunc/MathFunc.h"
#include "core/Input.h"
#include "Collision/CollisionManager.h"
#include "lib/myFunc/PrimitiveDrawer.h"

void Player::Initialize(Model* model){
	Character::Initialize(model);
}

void Player::Update(){
	///==============================
	//古い球の削除
	bullets_.remove_if([] (const std::unique_ptr<Bullet>& bullet){
		if (!bullet->GetIsActive()){
			CollisionManager::GetInstance()->RemoveCollider(bullet.get());
			return true;
		}
		return false;
					   });

	//===============================
	ReticleUpdate();
	Shoot();

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

void Player::ReticleUpdate(){
	// マウス座標を取得
	Vector2 mousePos = Input::GetMousePosition();

	// ビュープロジェクションビューポート合成行列
	Matrix4x4 matViewport = Matrix4x4::MakeViewportMatrix(0, 0, WinApp::kWindowWidth, WinApp::kWindowHeight, 0, 1);
	Matrix4x4 matVPV = Matrix4x4::Multiply(Matrix4x4::Multiply(pViewProjection_->matView, pViewProjection_->matProjection), matViewport);
	Matrix4x4 matInverseVPV = Matrix4x4::Inverse(matVPV);

	// スクリーン座標
	Vector3 posNear = Vector3(mousePos.x, mousePos.y, 0);
	Vector3 posFar = Vector3(mousePos.x, mousePos.y, 1);

	// スクリーン座標系からワールド座標系
	posNear = Matrix4x4::Transform(posNear, matInverseVPV);
	posFar = Matrix4x4::Transform(posFar, matInverseVPV);

	// マウスレイの方向
	Vector3 mouseDirection = posFar - posNear;
	// カメラから参照オブジェクトの距離
	const float kDistanceTestObject = 100.0f;
	reticlePos_ = posNear + (mouseDirection.Normalize() * kDistanceTestObject);
}



void Player::Draw(){
	if (isShoot_){
		PrimitiveDrawer::GetInstance()->DrawLine3d(pViewProjection_->transform.translate, reticlePos_, {1.0f,0.0f,0.0f,1.0f});
	}
}

void Player::Shoot(){
	isShoot_ = false;
	// スペースキーが押されているかチェックし、クールタイムが終了した場合のみ発射
	if (Input::PushKey(DIK_SPACE)){
		isShoot_ = true;
	}
}

void Player::OnCollision(Collider* other){

}

const Vector3 Player::GetCenterPos() const{
	const Vector3 offset = {0.0f,1.5f,0.0f};
	Vector3 worldPos = Matrix4x4::Transform(offset, model_->worldMatrix);
	return worldPos;
}

const Vector3 Player::GetForwardVector() const{
	return Vector3 {
		model_->worldMatrix.m[2][0], // 3列目（Z軸）のX成分
		model_->worldMatrix.m[2][1], // 3列目（Z軸）のY成分
		model_->worldMatrix.m[2][2]  // 3列目（Z軸）のZ成分
	}.Normalize();
}

const Vector3 Player::GetRightVector() const{
	return Vector3 {
		model_->worldMatrix.m[0][0], // 1列目（X軸）のX成分
		model_->worldMatrix.m[0][1], // 1列目（X軸）のY成分
		model_->worldMatrix.m[0][2]  // 1列目（X軸）のZ成分
	}.Normalize();
}