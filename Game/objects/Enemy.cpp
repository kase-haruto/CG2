#include "objects/Enemy.h" 
#include "Collision/CollisionManager.h"
#include "GameScene.h"

#include <cstdlib> // rand()を使用するために必要
#include <ctime>   // time()を使用して乱数をシードするために必要

Enemy::Enemy() {
	//衝突判定のid設定
	Collider::SetTypeID(static_cast< uint32_t >(CollisionTypeIdDef::kEnemy));
	CollisionManager::GetInstance()->AddCollider(this);
}
Enemy::~Enemy(){
	delete model_;
}


void Enemy::Initialize(Model* model){
	assert(!model);
	Character::Initialize(model_);
	model_->UpdateMatrix();

	
}

void Enemy::Initialize(Model* model, const Vector3& pos){
	model_ = model;
	model_->transform.translate = pos;

	// ランダムな速度の設定
	static bool isRandomInitialized = false;
	if (!isRandomInitialized){
		srand(static_cast< unsigned int >(time(nullptr)));
		isRandomInitialized = true;
	}

	// -0.2fから0.2fの範囲でランダムな速度を設定
	float randomX = ((rand() / static_cast< float >(RAND_MAX)) * 0.4f) - 0.2f;
	float randomY = ((rand() / static_cast< float >(RAND_MAX)) * 0.4f) - 0.2f;
	float randomZ = ((rand() / static_cast< float >(RAND_MAX)) * 0.4f) - 0.2f;

	velocity_ = Vector3(randomX, randomY, randomZ);
}

void Enemy::Update(){
	// 生存時間を過ぎたら削除
	lifeTime_--;
	if (lifeTime_ <= 0){
		isRemove_ = true;
	}

	// 死亡している場合の処理
	if (!isAlive_){
		// デプスなしに変更
		model_->IsNotDepth();

		Vector2 screenPos(1000.0f, 500.0f);

		// ビューポート行列を作成
		Matrix4x4 matViewport = Matrix4x4::MakeViewportMatrix(0, 0, WinApp::kWindowWidth, WinApp::kWindowHeight, 0, 1);

		// ビュープロジェクションビューポート合成行列を作成
		Matrix4x4 matVP = Matrix4x4::Multiply(pViewProjection_->matView, pViewProjection_->matProjection);
		Matrix4x4 matVPV = Matrix4x4::Multiply(matVP, matViewport);
		Matrix4x4 matInverseVPV = Matrix4x4::Inverse(matVPV);

		// スクリーン座標を設定
		Vector3 posNear = Vector3(screenPos.x, screenPos.y, 0);
		Vector3 posFar = Vector3(screenPos.x, screenPos.y, 1);

		// スクリーン座標系からワールド座標系への変換
		posNear = Matrix4x4::Transform(posNear, matInverseVPV);
		posFar = Matrix4x4::Transform(posFar, matInverseVPV);

		Vector3 direction = posFar - posNear;
		deadViewPos = posNear + (direction.Normalize() * fixedDistance);

		// モデル位置を設定
		model_->transform.translate = deadViewPos;

		// カメラ方向へのベクトルを計算
		Vector3 cameraPos = pViewProjection_->GetPos();
		Vector3 toCamera = cameraPos - deadViewPos;
		toCamera.Normalize();

		// x軸をカメラの方向に向けるための回転
		float rotationX = atan2(toCamera.y, sqrt(toCamera.x * toCamera.x + toCamera.z * toCamera.z));

		// y軸を回転させる
		static float rotationY = 0.0f;
		rotationY += 0.05f; // y軸回転速度

		// 回転を適用
		model_->transform.rotate = {0.0f, rotationY, 0.0};

		// 表示時間を減らし、タイムアウト時に削除
		if (--viewTime_ <= 0){
			isRemove_ = true;
		}
	} else{
		// 通常の移動処理
		Move();
	}

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
	if (isAlive_){
		// 位置を更新
		model_->transform.translate += velocity_;

		// 進行方向の回転を設定
		if (velocity_.Length() > 0.0f){ // velocity_が0でない場合のみ回転を設定
			velocity_.Normalize();

			// Y軸の回転角度を計算
			float rotationY = atan2(velocity_.x, velocity_.z); // XZ平面上の角度を計算

			// 回転をモデルに適用
			model_->transform.rotate.y = rotationY;
		}
	}
}


