#include "EnemySpawner.h"

//local
#include "EnemyManager.h"
#include "Game/3d/Player/PlayerAttack/IPlayerAttack.h"

//engine
#include "Engine/core/System.h"
#include "Engine/core/Math/Ease.h"
#include "Engine/collision/CollisionManager.h"
#include "lib/myFunc/MyFunc.h"
#include "Engine/graphics/camera/CameraManager.h"
#include "Engine/core/Json/JsonCoordinator.h"
#include "Engine/core/Audio/Audio.h"
//c++
#include <externals/imgui/imgui.h>

EnemySpawner::EnemySpawner(const std::string& modelName)
	:BaseGameObject(modelName){
	CollisionManager::GetInstance()->AddCollider(this);
	//オブジェクトの名前を設定
	BaseGameObject::SetName("EnemySpawner");
	Collider::SetName("EnemySpawner");

	core_ = std::make_unique<Model>("spawnerCore.obj");
	head_ = std::make_unique<Model>("spawnerHead.obj");

	core_->parent_ = &model_->transform;
	head_->parent_ = &model_->transform;
}

EnemySpawner::~EnemySpawner(){
	CollisionManager::GetInstance()->RemoveCollider(this);
}

////////////////////////////////////////////////////////////////////////
//                         main function
////////////////////////////////////////////////////////////////////////
void EnemySpawner::Initialize(){

	BaseGameObject::Initialize();

	//colliderの設定
	SphereCollider::Initialize(2.0f);
	Collider::type_ = ColliderType::Type_EnemySpawner;
	targetType_ = ColliderType::Type_PlayerAttack;


	// ランダムなフェーズオフセットを設定（0 ～ 2π の範囲）
	floatingPhaseOffset_ = Random::Generate(0.0f, 2.0f * 3.14159265f);

	spawnTimer_ = 0.0f;

	//回転用タイマーをランダムで
	rotateTimer_ = Random::Generate(1.0f, 5.0f);

	//回転方向をランダムで
	rotateDir_ = Random::Generate(0, 1) == 0 ? -1 : 1;

	BaseGameObject::Update();

	//パーティクルの初期化
	hitParticle_ = std::make_unique<HitParticle>();
	hitParticle_->Initialize("debugCube.obj", "white1x1.png");

	hp_ = std::make_unique<Sprite>("spawnHP.png");
	hp_->SetSize({100.0f,50.0f});

}

void EnemySpawner::Initialize(const Vector3& position){
	Initialize();
	model_->transform.translate = position;
}

void EnemySpawner::Update(){
	BaseGameObject::Update();


	//スポナーがアクティブでないときはreturn
	if (life_ <= 0){

		// activeじゃなくなった時の処理
		// 非アクティブ状態への移行がまだ開始されていなければ初期化
		if (!isTransitioning_){
			isTransitioning_ = true;
			rotationDone_ = false;
			transitionTimer_ = 0.0f;
			// 現在の位置と回転を保存
			initialY_ = model_->transform.translate.y;
			initialRotY_ = model_->transform.rotate.y;
		}

		float deltaTime = System::GetDeltaTime();

		// 回転フェーズ
		if (!rotationDone_){
			transitionTimer_ += deltaTime;
			float t = transitionTimer_ / rotationDuration_;
			if (t > 1.0f) t = 1.0f;  // tを0～1の範囲にクランプ

			// 適切なイージング関数を使用して回転を計算
			float easedT = EvoEase::EaseOutCubic(t);
			model_->transform.rotate.y = initialRotY_ + float(std::numbers::pi)*4 * easedT;

			// 回転が完了したら、次のフェーズに移行
			if (t >= 1.0f){
				rotationDone_ = true;
				transitionTimer_ = 0.0f;
				// 落下開始時に再度初期Y位置を設定
				initialY_ = model_->transform.translate.y;
			}
		}
		// 落下フェーズ
		else{
			transitionTimer_ += deltaTime;
			float t = transitionTimer_ / fallDuration_;
			if (t > 1.0f) t = 1.0f;  // tを0～1の範囲にクランプ

			// 適切なイージング関数を使用して下方向への移動を計算
			float easedT = EvoEase::EaseOutCubic(t);
			model_->transform.translate.y = initialY_ - fallDistance_ * easedT;

			// 落下が完了した場合の処理（必要に応じて追加）
			if (t >= 1.0f){
				isActive_ = false;
			}
		}
	} 

	spawnTimer_ += System::GetDeltaTime();

	if (isActive_){
		//6体よりも少ないときにスポーン
		if (enemyManager_->GetEnemyCount() < 6){
			// スポーン間隔を超えたら敵をスポーン
			if (spawnTimer_ >= spawnInterval_){
				if (enemyManager_){
					// EnemyManager を使ってスポナーの位置で敵を生成
					enemyManager_->SpawnEnemy("enemy.obj", GetCenterPos());
					spawnTimer_ = 0.0f;  // タイマーをリセット

				}
			}
		}

		// HP バーの最大幅（初期サイズ）
		const float maxHpBarWidth = 100.0f;
		const float minHpBarWidth = 0.0f; // HP が 0 の場合の幅

		// 現在の `life_` に基づいて横幅を線形補間で計算
		float lifeRatio = static_cast< float >(life_) / static_cast< float >(maxLife_);
		float currentWidth = Lerp(minHpBarWidth, maxHpBarWidth, lifeRatio);

		// HP バーのサイズを更新
		hp_->SetSize({currentWidth, 50.0f});

		//座標を更新
		Vector3 offset(0.0f, 2.0f, 0.0f);
		Vector3 worldPos = GetCenterPos()+offset;
		Vector2 screenPos = WorldToScreen(worldPos);
		hp_->SetPosition(screenPos);
		hp_->Update();

		//中心のコアを回転
		core_->transform.rotate.y += 0.02f;

		////////////////////////////////////////////////////////////////////////
		//		headを上下運動
		////////////////////////////////////////////////////////////////////////
		static float globalTimeAccumulator = 0.0f;  // 全敵共通の経過時間
		float deltaTime = System::GetDeltaTime();
		globalTimeAccumulator += deltaTime;

		// フローティングのパラメータ
		const float amplitude = 0.2f;  // 振幅（上下の移動範囲）
		const float frequency = 1.0f;  // 周波数（1秒あたりの揺れの回数）

		// 各敵の個別フェーズを考慮したサイン波による上下運動
		float offsetY = amplitude * std::sin(frequency * globalTimeAccumulator + floatingPhaseOffset_);
		// 現在の位置を取得し、新しいY座標を設定
		head_->transform.translate.y = offsetY;

		//回転
		rotateTimer_ -= System::GetDeltaTime();
		if (rotateTimer_ <= 0.0f){
			rotateTimer_ = Random::Generate(1.0f, 5.0f);
			rotateDir_ = Random::Generate(0, 1) == 0 ? -1 : 1;
		}
		head_->transform.rotate.y += 0.02f * rotateDir_;

	}

	//パーティクルの更新
	hitParticle_->SetEmitPos(GetCenterPos());
	hitParticle_->Update();

	////////////////////////////////////////////////////////////////////////
	//		それぞれの更新
	////////////////////////////////////////////////////////////////////////
	core_->Update();
	head_->Update();

	//colliderの更新
	shape_.center = GetCenterPos();


}

void EnemySpawner::Draw(){
		SphereCollider::Draw();
		BaseGameObject::Draw();

		core_->Draw();
		head_->Draw();
}

void EnemySpawner::DrawUI(){
	Vector2 screenPos;
	if (WorldToScreen(GetCenterPos(), screenPos)){
		// カメラに見えている場合に描画
		hp_->SetPosition(screenPos);
		hp_->Draw();
	}
}


////////////////////////////////////////////////////////////////////////
//                         ui/ImGui
////////////////////////////////////////////////////////////////////////
void EnemySpawner::ShowGui(){
	SceneObject::ShowGui();

	ImGui::Spacing(); // 少しスペースを追加

	// BaseGameObjectのGUI表示
	BaseGameObject::ShowGui();

	// BoxColliderのGUI表示
	SphereCollider::ShowGui();
}

////////////////////////////////////////////////////////////////////////
//                         collision
////////////////////////////////////////////////////////////////////////
void EnemySpawner::OnCollisionEnter([[maybe_unused]] Collider* other){
	//* 衝突相手がtargetType_に含まれていなければreturn
	if ((other->GetType() & Collider::GetTargetType()) != ColliderType::Type_None){


		//////////////////////////////////////////////////////////////////
		//				playerの攻撃と衝突
		//////////////////////////////////////////////////////////////////
		if (other->GetType() == ColliderType::Type_PlayerAttack){
			// IPlayerAttack型にキャスト
			IPlayerAttack* playerAttack = dynamic_cast< IPlayerAttack* >(other);

			if (playerAttack){
				Vector3 playerPos = playerAttack->GetPlayerPos();

				life_ -= playerAttack->GetDamage();

				hitParticle_->Emit(4);

				Audio::Play("hit.mp3", false);
			}
		}
	}
}

void EnemySpawner::OnCollisionStay([[maybe_unused]] Collider* other){}

void EnemySpawner::OnCollisionExit([[maybe_unused]] Collider* other){}
