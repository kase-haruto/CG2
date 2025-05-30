#include "EnemyCollection.h"

#include <Engine/Foundation/Clock/ClockManager.h>
#include <Engine/Scene/Utirity/SceneUtility.h>
#include <Engine/Foundation/Utility/Random/Random.h>
#include <externals/imgui/imgui.h>

EnemyCollection::EnemyCollection(SceneContext* context):
sceneContext_(context){
	SetName("EnemyCollection", ObjectType::GameObject);
}

/////////////////////////////////////////////////////////////////////////////////////////
//		更新
/////////////////////////////////////////////////////////////////////////////////////////
void EnemyCollection::Update(){

	for (auto& enemy : enemies_){
		enemy->Update();
	}

	// エネミーの削除
	enemies_.remove_if([](const std::unique_ptr<Enemy>& enemy) {
		return !enemy->GetIsAlive();
	});

	//　エネミーのスポーン
	Spawn(ClockManager::GetInstance()->GetDeltaTime());
}

void EnemyCollection::ShowGui(){
	ImGui::Text("Enemy Count : %d", enemies_.size());
	ImGui::SeparatorText("Spawn");
	ImGui::DragFloat3("SpawnPos", &spawnPos_.x, 0.01f);
	ImGui::InputFloat("SpawnInterval", &spawnInterval_);
	ImGui::SeparatorText("Enemy");
	//for (auto& enemy : enemies_){
	//	enemy->ShowImGui();
	//}
}

///////////////////////////////////////////////////////////////////////////////////////////
//		生成
///////////////////////////////////////////////////////////////////////////////////////////
void EnemyCollection::Spawn(float deltaTime) {
	if (spawnTimer_ > spawnInterval_) {
		spawnTimer_ = 0.0f;

		std::unique_ptr<Enemy> enemy;
		if (sceneContext_) {
			CreateAndAddObject<Enemy>(sceneContext_, enemy, "debugCube.obj", "enemy");
			sceneContext_->GetMeshRenderer()->Register(enemy->GetModel(), &enemy->GetWorldTransform());
		} else {
			enemy = std::make_unique<Enemy>("debugCube.obj", "enemy");
		}

		// Y座標は 10±1 でランダム
		float randomY = 10.0f + Random::Generate(-1.0f, 1.0f);
		// X座標は -5.0f ～ +5.0f でランダム
		float randomX = Random::Generate(-5.0f, 5.0f);

		// スポーン位置
		Vector3 spawnPos(randomX, randomY, currentSpawnZ_);
		enemy->SetPosition(spawnPos);

		// 次のスポーンZを更新
		currentSpawnZ_ += spawnZStep_;
		if (currentSpawnZ_ > spawnZEnd_) {
			currentSpawnZ_ = spawnZStart_; // ループさせる
		}

		enemies_.emplace_back(std::move(enemy));
	}
	spawnTimer_ += deltaTime;
}

///////////////////////////////////////////////////////////////////////////////////////////
//		追加
///////////////////////////////////////////////////////////////////////////////////////////
void EnemyCollection::AddEnemy(Enemy* enemy){
	if (enemy){
		enemies_.emplace_back(std::move(enemy));
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
//		削除
/////////////////////////////////////////////////////////////////////////////////////////
void EnemyCollection::RemoveEnemy([[maybe_unused]] size_t index){}

/////////////////////////////////////////////////////////////////////////////////////////
//		クリア
/////////////////////////////////////////////////////////////////////////////////////////
void EnemyCollection::Clear(){}
