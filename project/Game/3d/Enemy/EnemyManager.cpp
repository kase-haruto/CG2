#include "EnemyManager.h"

#include "Engine/core/System.h"


////////////////////////////////////////////////////////////////////////////////////////////////////
//                      main functions
////////////////////////////////////////////////////////////////////////////////////////////////////
void EnemyManager::Initialize(){
    enemies_.clear();      // 敵リストの初期化
    elapsedTime_ = 0.0f;   // 累積時間のリセット
}

void EnemyManager::Update(){
    // 毎フレームのデルタタイムを取得
    float deltaTime = System::GetDeltaTime();

    // 累積時間を更新
    elapsedTime_ += deltaTime;

    // 敵リストの更新
    for (auto it = enemies_.begin(); it != enemies_.end(); ){
        auto& enemy = *it;
        if (!enemy->GetIsAlive()){
            // 敵が死んでいたら削除
            it = enemies_.erase(it);
        } else{
            // 更新
            enemy->Update();
            ++it;
        }
    }

    // 敵の数が3以下で、10秒以上経過していたら敵をスポーン
    if (enemies_.size() <= maxEnemies_ && elapsedTime_ >= spawnInterval_){
        SpawnEnemy("enemy.obj", Vector3(0.0f, 3.0f, 0.0f)); // スポーン位置は例
        elapsedTime_ = 0.0f; // 累積時間をリセット
    }
}

void EnemyManager::Draw(){
    for (const auto& enemy : enemies_){
        enemy->Draw();
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//                      helper functions
////////////////////////////////////////////////////////////////////////////////////////////////////
void EnemyManager::SpawnEnemy(const std::string& modelName, [[maybe_unused]]const Vector3& position){
    // ユニークポインタで敵を生成し、リストに追加
    auto newEnemy = std::make_unique<Enemy>(modelName);
    newEnemy->Initialize();
	newEnemy->SetPosition(position);
    enemies_.push_back(std::move(newEnemy));
}

void EnemyManager::RemoveEnemy(Enemy* enemy){
    // リストから指定の敵を削除
    enemies_.erase(
        std::remove_if(enemies_.begin(), enemies_.end(),
        [enemy] (const std::unique_ptr<Enemy>& e){ return e.get() == enemy; }),
        enemies_.end()
    );
}