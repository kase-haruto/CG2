#pragma once
/* ========================================================================
/*	include space
/* ===================================================================== */
#include <Game/3dObject/Actor/Enemy/Enemy.h>

class SceneContext;

class EnemyCollection :
	public SceneObject{
public:
	//===================================================================*/
	//                      Public Methods
	//===================================================================*/
	EnemyCollection(const std::string& name = "EnemyCollection");
	~EnemyCollection() = default;

	void Update() override;
	void ShowGui() override;

	void SetSceneContext(SceneContext* context);
	void SetPlayerTransform(WorldTransform* pTransform);

	void AddEnemy(Enemy* enemy);
	void AddSpawner(class EnemySpawner* spawner);

	void CreateSpawners();

	void Clear();

private:
	//===================================================================*/
	//                      Private variables
	//===================================================================*/
	std::list<Enemy*> enemies_;
	std::vector<class EnemySpawner*> spawners_;

	SceneContext* sceneContext_ = nullptr;
	WorldTransform* playerTransform_ = nullptr;
};

