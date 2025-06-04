#pragma once
/* ========================================================================
/*	include space
/* ===================================================================== */
#include <Game/3dObject/Actor/Enemy/Enemy.h>
#include <Engine/Scene/Context/SceneContext.h>

class EnemyCollection :
	SceneObject{
public:
	//===================================================================*/
	//                      Public Methods
	//===================================================================*/
	EnemyCollection(SceneContext* context);
	~EnemyCollection() = default;
	void Update();
	void ShowGui()override;
private:
	//===================================================================*/
	//                      Protected Methods
	//===================================================================*/
	void Spawn(float deltaTime);
	void AddEnemy(Enemy* enemy);
	void RemoveEnemy(size_t index);
	void Clear();

	// config ============================================================*/

private:
	//===================================================================*/
	//                      Private variables
	//===================================================================*/
	std::list<std::unique_ptr<Enemy>> enemies_; // 登録された全エネミー

	SceneContext* sceneContext_ = nullptr; // シーンコンテキスト

	float spawnInterval_ = 1.0f; // スポーン間隔
	float spawnTimer_ = 0.0f;   // スポーンタイマー
	Vector3 spawnPos_ = {0.0f, 0.0f, 0.0f}; // スポーン位置

	float spawnZStart_ = 0.0f; // 開始位置
	float spawnZEnd_ = 799.0f;    // 終了位置
	float spawnZStep_ = 20.0f;    // スポーン間隔
	float currentSpawnZ_;
};

