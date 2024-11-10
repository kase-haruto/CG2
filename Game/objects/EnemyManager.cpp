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
		if (enemy->GetIsRemove()){
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
void EnemyManager::AddEnemy(const Vector3& pos){

	// プレイヤーの前方ベクトルに基づいて敵の位置を決定
	Vector3 spawnPos = pos;

	// unique_ptrでModelを作成
	auto enemyModel = std::make_unique<Model>("teapot");

	// Enemyオブジェクトを作成し、生ポインタを渡して初期化
	auto enemy = std::make_unique<Enemy>();
	enemy->Initialize(enemyModel.get(), spawnPos); // 生ポインタを渡す
	enemy->SetViewProjection(pViewProjection_);

	enemies_.emplace_back(std::move(enemy));

	// enemies_がモデルの所有権を持たない場合、unique_ptrの解放を避けるためreleaseを使用
	enemyModel.release();
}