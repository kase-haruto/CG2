#include "Enemy.h"

#include "../Player/Player.h"
#include "../Player/PlayerAttack/IPlayerAttack.h"

#include "Engine/collision/CollisionManager.h"
#include "Engine/core/Json/JsonCoordinator.h"
#include <externals/imgui/imgui.h>

Enemy::Enemy(const std::string& modelName)
:Character(modelName){
	CollisionManager::GetInstance()->AddCollider(this);
}

Enemy::~Enemy(){
	CollisionManager::GetInstance()->RemoveCollider(this);
}


/////////////////////////////////////////////////////////////////////////////////////
//					mainFunction
/////////////////////////////////////////////////////////////////////////////////////
void Enemy::Initialize(){

	BaseGameObject::SetName("Enemy");
	Collider::SetName("Enemy");
	
	//コライダー
	BaseGameObject::Initialize();

	BoxCollider::Initialize(model_->transform.scale);
	
	Collider::type_ = ColliderType::Type_Enemy;
	Collider::targetType_ = 
		ColliderType::Type_Player |
		ColliderType::Type_PlayerAttack;

	moveSpeed_ = 2.0f;

	JsonCoordinator::LoadGroup(BaseGameObject::GetName(), BaseGameObject::jsonPath);
}

void Enemy::Update(){

	//衝突処理の更新
	shape_.center = GetCenterPos();
	shape_.rotate = model_->transform.rotate;

	//モデルの
	BaseGameObject::Update();
	//落下処理
	Character::Update();
}

void Enemy::Draw(){
	BoxCollider::Draw();
	BaseGameObject::Draw();
}


/////////////////////////////////////////////////////////////////////////////////////
//					Collision
/////////////////////////////////////////////////////////////////////////////////////
void Enemy::OnCollisionEnter([[maybe_unused]]Collider* other){
	
	//* 衝突相手がtargetType_に含まれていなければreturn
	if ((other->GetType() & Collider::GetTargetType()) != ColliderType::Type_None){

		//////////////////////////////////////////////////////////////////
		//				playerと衝突
		//////////////////////////////////////////////////////////////////
		if (other->GetType() == ColliderType::Type_Player){



		}

		//////////////////////////////////////////////////////////////////
		//				playerの攻撃と衝突
		//////////////////////////////////////////////////////////////////
		else if(other->GetType() == ColliderType::Type_PlayerAttack){
			// IPlayerAttack型にキャスト
			IPlayerAttack* playerAttack = dynamic_cast< IPlayerAttack* >(other);

			if (playerAttack){
				Vector3 playerPos = playerAttack->GetPlayerPos();

				// ノックバック処理
				// プレイヤーと反対方向に力を加える
				Vector3 dir = (GetCenterPos() - playerPos).Normalize();
				float knockbackForce = 4.0f; // 力の大きさ（必要に応じて調整）
				float knockbackDuration = 0.5f; // 持続時間（秒単位、必要に応じて調整）

				KnockBack(dir, knockbackForce, knockbackDuration);
			}
		}

	}

}

void Enemy::OnCollisionStay([[maybe_unused]] Collider* other){}

void Enemy::OnCollisionExit([[maybe_unused]] Collider* other){}

/////////////////////////////////////////////////////////////////////////////////////
//					ui/ImGui
/////////////////////////////////////////////////////////////////////////////////////
void Enemy::ShowGui(){

#ifdef _DEBUG
	SceneObject::ShowGui();

	if (ImGui::Button("save")){
		JsonCoordinator::SaveGroup(BaseGameObject::GetName(), BaseGameObject::jsonPath);
	}

	ImGui::Separator();

	BaseGameObject::ShowGui();

	ImGui::Separator();

	BoxCollider::ShowGui();
#endif // _DEBUG

}