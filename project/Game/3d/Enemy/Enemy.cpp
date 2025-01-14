#include "Enemy.h"

#include "Engine/core/System.h"
#include "Engine/core/Audio/Audio.h"

#include "../Player/Player.h"
#include "../Player/PlayerAttack/IPlayerAttack.h"

#include "Engine/core/Clock/ClockManager.h"

#include "Engine/collision/CollisionManager.h"
#include "Engine/core/Json/JsonCoordinator.h"
#include <externals/imgui/imgui.h>
#include "lib/myFunc/Random.h"

#include <numbers>

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
		ColliderType::Type_PlayerAttack|
		ColliderType::Type_Enemy;

	moveSpeed_ = 2.0f;

	JsonCoordinator::LoadGroup(BaseGameObject::GetName(), BaseGameObject::jsonPath);

	life_ = 600;

	//モデルの
	BaseGameObject::Update();

	hitParticle_ = std::make_unique<AttackParticle>();
	hitParticle_->Initialize("plane.obj", "attackParticle.png");

	hitParticle2_ = std::make_unique<HitParticle>();
	hitParticle2_->Initialize("debugCube.obj", "white1x1.png");

	// ランダムなフェーズオフセットを設定（0 ～ 2π の範囲）
	floatingPhaseOffset_ = Random::Generate(0.0f, 2.0f * 3.14159265f);

}

void Enemy::Update(){

	if (life_ <= 0){
		isAlive_ = false;
	}

	Move();
	Floating();

	hitParticle_->SetEmitPos(GetCenterPos());
	hitParticle_->Update();

	hitParticle2_->SetEmitPos(GetCenterPos());
	hitParticle2_->Update();

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


void Enemy::Move(){
	if (target_){
		// 現在の中心位置とターゲットの位置を取得
		Vector3 currentPosition = GetCenterPos();
		Vector3 targetPosition = *target_;

		// ターゲットへの方向ベクトルを計算
		Vector3 direction = (targetPosition - currentPosition).Normalize();
		float distance = (targetPosition - currentPosition).Length();

		if (distance > 5.0f){ // 一定距離以上離れている場合に移動

			// 移動量を計算（フレームレートに依存しないようにデルタタイムを使用）
			
			direction.y = 0.0f; // 上下方向の移動はしない
			Vector3 movement = direction * movementSpeed_ ;

			// モデルの位置を更新
			model_->transform.translate += movement * ClockManager::GetInstance()->GetDeltaTime();
		}

		// 進行方向を向くように回転を更新
		float targetRotationY = std::atan2(-direction.x, -direction.z);
		// 回転を設定
		model_->transform.rotate.y = targetRotationY;
	}
}

void Enemy::Floating(){
	// 浮遊動作の設定
	static float globalTimeAccumulator = 0.0f;  // 全敵共通の経過時間
	float deltaTime = ClockManager::GetInstance()->GetDeltaTime();
	globalTimeAccumulator += deltaTime;

	// フローティングのパラメータ
	const float amplitude = 0.5f;  // 振幅（上下の移動範囲）
	const float frequency = 1.0f;  // 周波数（1秒あたりの揺れの回数）

	// 各敵の個別フェーズを考慮したサイン波による上下運動
	float offsetY = amplitude * std::sin(frequency * globalTimeAccumulator + floatingPhaseOffset_);

	// 現在の位置を取得し、新しいY座標を設定
	Vector3 currentTranslate = model_->transform.translate;
	currentTranslate.y = offsetY;
	model_->transform.translate = currentTranslate;
}


/////////////////////////////////////////////////////////////////////////////////////
//					Collision
/////////////////////////////////////////////////////////////////////////////////////
void Enemy::OnCollisionEnter([[maybe_unused]] Collider* other){

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
		else if (other->GetType() == ColliderType::Type_PlayerAttack){
			// IPlayerAttack型にキャスト
			IPlayerAttack* playerAttack = dynamic_cast< IPlayerAttack* >(other);

			if (playerAttack){
				Vector3 playerPos = playerAttack->GetPlayerPos();

				// ノックバック処理
				// プレイヤーと反対方向に力を加える
				Vector3 dir = (GetCenterPos() - playerPos).Normalize();
				float knockbackForce = 5.0f; // 力の大きさ（必要に応じて調整）
				float knockbackDuration = 0.5f; // 持続時間（秒単位、必要に応じて調整）

				KnockBack(dir, knockbackForce, knockbackDuration);

				life_ -= playerAttack->GetDamage();

				hitParticle_->Emit(2);
				hitParticle2_->Emit(4);

				Audio::Play("hit.mp3", false);
			}
		}
	}

}


void Enemy::OnCollisionStay([[maybe_unused]] Collider* other){
	//* 衝突相手がtargetType_に含まれていなければreturn
	if ((other->GetType() & Collider::GetTargetType()) == ColliderType::Type_None){
		return;
	}

	//////////////////////////////////////////////////////////////////
	// エネミー同士の衝突時の分離処理
	//////////////////////////////////////////////////////////////////
	if (other->GetType() == ColliderType::Type_Enemy){
		// 他のエネミーと重なっている場合、分離させる
		Enemy* otherEnemy = dynamic_cast< Enemy* >(other);
		if (otherEnemy && otherEnemy != this){
			Vector3 otherPosition = otherEnemy->GetCenterPos();
			Vector3 currentPosition = GetCenterPos();
			Vector3 separationDir = currentPosition - otherPosition; // 他との分離方向
			float distance = separationDir.Length();

			// 最小許容距離を設定（例えば2.0fなど適宜調整）
			const float minDistance = 2.0f;
			if (distance < minDistance && distance > 0.001f){
				separationDir.Normalize();
				// 衝突からの逃避距離を決定（距離が短いほど大きく押し出す）
				float overlap = minDistance - distance;
				// 分離処理の強さを調整（シーンに応じてパラメータを変更）
				float separationStrength = 0.1f;
				Vector3 separationMovement = separationDir * overlap * separationStrength;

				// モデルの位置を更新して重なりを解消
				model_->transform.translate += separationMovement;
			}
		}
	}

	//////////////////////////////////////////////////////////////////
	// 他の処理があればここに記述
	//////////////////////////////////////////////////////////////////
}



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

