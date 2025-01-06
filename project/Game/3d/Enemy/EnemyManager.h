#pragma once

#include "Enemy.h"
#include <vector>
#include <memory>

class EnemyManager{
public:
	//===================================================================*/
	//      public methods
	//===================================================================*/
	EnemyManager() = default;
	~EnemyManager() = default;
    // 初期化
    void Initialize();

    // 毎フレーム更新
    void Update();

    // 描画
    void Draw();

    // 敵をスポーンする
    void SpawnEnemy(const std::string& modelName, const Vector3& position);

    // 敵を削除する
    void RemoveEnemy(Enemy* enemy);

private:
	//===================================================================*/
	//      private variables
	//===================================================================*/
    // 敵リスト
    std::vector<std::unique_ptr<Enemy>> enemies_;

    // 累積時間（秒）
    float elapsedTime_ = 0.0f;

    // スポーン間隔（秒）
    static constexpr float spawnInterval_ = 10.0f;

    // 敵の最大数制御
    static constexpr size_t maxEnemies_ = 3;
};
