#include "objects/EnemyManager.h" 
#include "Collision/CollisionManager.h"

EnemyManager::EnemyManager() { 
	enemies_.clear();
} 

EnemyManager::~EnemyManager() { 
	
}


void EnemyManager::Initialize(){}

void EnemyManager::Update(){
	enemies_.remove_if([] (const std::unique_ptr<Enemy>& enemy){
		if (!enemy->GetIsAlive()){
			CollisionManager::GetInstance()->RemoveCollider(enemy.get());
			return true;
		}
		return false;
					   });


	for (auto& enemy: enemies_){
  		enemy->Update();
	}
}

void EnemyManager::Draw(){
	for (auto& enemy : enemies_){
		enemy->Draw();
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
//		リストに敵の追加
/////////////////////////////////////////////////////////////////////////////////////////
void EnemyManager::AddEnemy(const Vector3& pos, const Vector3& forwardVector, float distanceAhead){
	// プレイヤーの前方ベクトルに基づいて敵の位置を決定
	Vector3 spawnPos = pos + forwardVector * distanceAhead;

	Model* enemyModel = new Model("teapot");
	enemyModel->SetViewProjection(pViewProjection_);

	std::unique_ptr<Enemy> enemy = std::make_unique<Enemy>();
	enemy->Initialize(std::move(enemyModel), spawnPos);


	enemies_.emplace_back(std::move(enemy));
}

