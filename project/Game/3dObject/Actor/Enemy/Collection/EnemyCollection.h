#pragma once
/* ========================================================================
/*	include space
/* ===================================================================== */
#include <Game/3dObject/Actor/Enemy/Enemy.h>

class EnemyCollection :
	SceneObject{
public:
	//===================================================================*/
	//                      Public Methods
	//===================================================================*/
	EnemyCollection(std::function<void(IMeshRenderable*)> registerCB);
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

private:
	//===================================================================*/
	//                      Private variables
	//===================================================================*/
	std::list<std::unique_ptr<Enemy>> enemies_; // 登録された全エネミー
	std::function<void(IMeshRenderable*)> registerCB_;

	float spawnInterval_ = 2.0f; // スポーン間隔
	float spawnTimer_ = 0.0f;   // スポーンタイマー
	Vector3 spawnPos_ = {0.0f, 0.0f, 0.0f}; // スポーン位置
};

