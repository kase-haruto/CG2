#include "objects/Bullet.h"

#include "Collision/CollisionManager.h"


Bullet::Bullet(){
	//衝突判定のid設定
	Collider::SetTypeID(static_cast< uint32_t >(CollisionTypeIdDef::kBullet));

	CollisionManager::GetInstance()->AddCollider(this);
}

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

void Bullet::OnCollision(Collider* other){
	// 衝突相手の種別IDを取得
	uint32_t typeID = other->GetTypeID();

	//敵と衝突したら消す
	if (typeID == static_cast< uint32_t >(CollisionTypeIdDef::kEnemy)){
		isActive_ = false;
	}

}

void Bullet::SetViewProjection(const ViewProjection* viewProjection){
	if (!model_){
		return;
	}
	model_->SetViewProjection(viewProjection);
}

const Vector3 Bullet::GetCenterPos() const{
	const Vector3 offset = {0.0f,0.0f,0.0f};
	Vector3 worldPos = Matrix4x4::Transform(offset, model_->worldMatrix);
	return worldPos;
}
