#include "EnemySpawner.h"

#include <Game/3dObject/Actor/Enemy/Collection/EnemyCollection.h>
#include <Engine/Scene/Context/SceneContext.h>

EnemySpawner::EnemySpawner(const std::string& name) {
	SetName(name, ObjectType::GameObject);
	worldTransform_.Initialize();
	worldTransform_.translation = { 0.0f, 0.0f, 5.0f };

}

void EnemySpawner::Update() {
	float dt = ClockManager::GetInstance()->GetDeltaTime();

	// 回転（rotationDir_ に従って回転）
	Vector3 rot = rotationDir_ * rotationSpeed_ * dt;
	worldTransform_.eulerRotation += rot;

	worldTransform_.rotationSource = RotationSource::Euler;
	worldTransform_.Update();

	spawnTimer_ += dt;
		if (spawnTimer_ >= spawnInterval_) {
			Spawn();
			spawnTimer_ = 0.0f;
		}

}

void EnemySpawner::ShowGui() {
	ImGui::SeparatorText(GetName().c_str());
	worldTransform_.ShowImGui();
	ImGui::DragFloat("Rotation Speed", &rotationSpeed_, 0.1f);
	ImGui::DragFloat3("Rotation Dir", &rotationDir_.x, 0.1f);
	ImGui::InputFloat("Spawn Interval", &spawnInterval_);
	ImGui::DragFloat3("Spawn Area Min", &spawnAreaMin_.x, 0.1f);
	ImGui::DragFloat3("Spawn Area Max", &spawnAreaMax_.x, 0.1f);
}

void EnemySpawner::SetSceneContext(SceneContext* context) { sceneContext_ = context; }


void EnemySpawner::SetPlayerTransform(WorldTransform* playerTransform) {
	playerTransform_ = playerTransform;
	worldTransform_.parent = playerTransform->parent;
}

void EnemySpawner::Spawn() {
	if (!sceneContext_ || !ownerCollection_) return;

	// 現在の有効な敵の数を数える
	size_t aliveCount = 0;
	for (auto it = spawnedEnemies_.begin(); it != spawnedEnemies_.end();) {
		if (!(*it) || !(*it)->GetIsAlive()) {
			it = spawnedEnemies_.erase(it);
		} else {
			++it;
			++aliveCount;
		}
	}

	// 最大数に達していたらスポーンしない
	if (aliveCount >= maxSpawnCount_) return;

	// 新規スポーン
	Enemy* enemy = sceneContext_->GetObjectLibrary()->CreateAndAddObject<Enemy>("debugCube.obj", "enemy");
	if (!enemy) return;

	Vector3 localOffset = Random::GenerateVector3(spawnAreaMin_, spawnAreaMax_);
	enemy->SetPosition(localOffset);
	enemy->SetParent(&worldTransform_);

	ownerCollection_->AddEnemy(enemy);
	spawnedEnemies_.push_back(enemy);
}