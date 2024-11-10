#include "objects/Player.h"
#include "myFunc/MyFunc.h"
#include "myFunc/MathFunc.h"
#include "core/Input.h"
#include "Collision/CollisionManager.h"

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
	const float kDistanceTestObject = 70.0f;
	reticlePos_ = posNear + (mouseDirection.Normalize() * kDistanceTestObject);
}



void Player::Draw(){
	for (auto& bullet : bullets_){
		bullet->Draw();
	}
}



void Player::Shoot(){
	if (Input::TriggerKey(DIK_SPACE)){
		// 弾の速度（必要に応じて調整可能）
		const float kBulletSpeed = 1.0f;

		// プレイヤーのワールド位置を取得（もしくは銃口の位置）
		Vector3 playerPosition = this->GetWorldPosition(); // プレイヤーのワールド位置を取得

		// プレイヤー位置からレティクル位置への方向ベクトルを計算
		Vector3 directionToReticle = reticlePos_ - playerPosition;
		directionToReticle = directionToReticle.Normalize(); // 方向ベクトルを正規化

		// 方向に速度を掛ける
		Vector3 bulletVelocity = directionToReticle * kBulletSpeed;

		// 弾を生成してユニークポインタにラップ
		Model* model = new Model("debugCube");
		std::unique_ptr<Bullet> newBullet = std::make_unique<Bullet>();
		model->SetViewProjection(pViewProjection_);

		// 弾の初期化（位置と速度を設定）
		newBullet->Initialize(std::move(model), playerPosition, bulletVelocity);

		// 弾を登録
		bullets_.push_back(std::move(newBullet));
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

const Vector3 Player::GetRightVector() const {
    return Vector3 {
        model_->worldMatrix.m[0][0], // 1列目（X軸）のX成分
        model_->worldMatrix.m[0][1], // 1列目（X軸）のY成分
        model_->worldMatrix.m[0][2]  // 1列目（X軸）のZ成分
    }.Normalize();
}