#include "Enemy.h"

#include "Game/3d/gameObject/Player/Attack/IPlayerAttack.h"
#include "Engine/Collision/CollisionManager.h"

// lib
#include "lib/myFunc/MyFunc.h"
#include <algorithm>
#undef max

Enemy::Enemy(const std::string& modelName)
	:BaseGameObject(modelName){

	SphereCollider::Initialize(model_->transform.scale.x * 0.5f + 0.1f);

	SphereCollider::colliderType_ = ColliderType::Type_Enemy;
	SphereCollider::targetColliderType_ = ColliderType::Type_Player | ColliderType::Type_PlayerAttack;

	CollisionManager::GetInstance()->AddCollider(this);
}

Enemy::~Enemy(){

	CollisionManager::GetInstance()->RemoveCollider(this);

}

/////////////////////////////////////////////////////////////////////////
//						main fanc
/////////////////////////////////////////////////////////////////////////
void Enemy::Initialize(){

	BaseGameObject::SetName("enemy");
	SphereCollider::SetName("enemy");
	BaseGameObject::Initialize();

}

void Enemy::Update(){

	//ノックバック速度の更新
	UpdateVelocities();


	//最終的な速度の計算
	totalVelocity_ = moveVelocity_ + knockbackVelocity_+gravityVelocity_;

	// 座標の更新
	model_->transform.translate += totalVelocity_ * deltaTime;
	shape_.center = model_->transform.translate;

	// 接地状態の更新
	if (model_->transform.translate.y <= 0.0f){ // 地面の高さが 0 と仮定
		model_->transform.translate.y = 0.0f;    // 地面に固定
		isGrounded_ = true;                      // 接地状態にする
	} else{
		isGrounded_ = false; // 空中にいる場合
	}

	BaseGameObject::Update();

}

void Enemy::Draw(){

	//SphereCollider::Draw();
	BaseGameObject::Draw();

}

void Enemy::Move(){}


void Enemy::UpdateVelocities(){
	// ノックバック速度の減衰
	if (knockbackVelocity_.Length() > 0.01f){
		knockbackVelocity_ *= 0.95f; // 徐々に減衰
	} else{
		knockbackVelocity_ = Vector3(0.0f, 0.0f, 0.0f);
	}

	// 重力の適用
	if (!isGrounded_){ // 空中にいる場合のみ重力を適用
		gravityVelocity_ += acceleration_ * deltaTime; // 重力加速度を加算
	} else{
		gravityVelocity_ = Vector3(0.0f, 0.0f, 0.0f); // 接地している場合はリセット
	}
}



/////////////////////////////////////////////////////////////////////////
//						imgui / ui
/////////////////////////////////////////////////////////////////////////
void Enemy::ImGui(){}

/////////////////////////////////////////////////////////////////////////
//						collision
/////////////////////////////////////////////////////////////////////////
void Enemy::OnCollisionEnter([[maybe_unused]] Collider* other){

	//* 衝突相手がtargetType_に含まれていなければreturn
	if ((other->GetType() & Collider::GetTargetType()) != ColliderType::Type_None){

		switch (other->GetType()){

			//===================================================================*/
			//                    VS Player
			//===================================================================*/
			case ColliderType::Type_Player:
			{



			}
			break;

			//===================================================================*/
			//                    VS PlayerAttack
			//===================================================================*/
			case ColliderType::Type_PlayerAttack:
			{
				auto attackColl = dynamic_cast< IPlayerAttack* >(other);
				if (!attackColl){
					return; // キャスト失敗時は何もしない
				}

				float force = attackColl->GetForce();
				if (force <= 0.0f){
					return; // 力が無効なら処理を中断
				}

				// ノックバックの方向を計算
				Vector3 knockbackDir = GetCenterPos() - attackColl->GetPlayerPos();
				if (knockbackDir.Length() > 0.0f){
					knockbackDir = knockbackDir.Normalize();
				} else{
					knockbackDir = Vector3(0.0f, 1.0f, 0.0f); // デフォルトの上方向
				}

				// 高さ成分の調整
				knockbackDir.y = std::max(knockbackDir.y, 2.0f);

				// ノックバック速度を計算
				knockbackVelocity_ = knockbackDir * force;
			}


		}

	}

}

void Enemy::OnCollisionStay([[maybe_unused]] Collider* other){}

void Enemy::OnCollisionExit([[maybe_unused]] Collider* other){}


/////////////////////////////////////////////////////////////////////////
//						getter / setter
/////////////////////////////////////////////////////////////////////////
const Vector3 Enemy::GetCenterPos() const{
	const Vector3 offset = {0.0f,0.5f,0.0f};
	Vector3 worldPos = Vector3::Transform(offset, model_->worldMatrix);
	return worldPos;
}