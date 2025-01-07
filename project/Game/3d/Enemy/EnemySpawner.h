#pragma once
#include "Engine/objects/Collider/SphereCollider.h"
#include "Game/3d/BaseGameObject.h"
#include <cstdint>

// Forward declaration
class EnemyManager;

class EnemySpawner
    : public BaseGameObject, public SphereCollider{
public:
    EnemySpawner() = default;
    EnemySpawner(const std::string& modelName);
    ~EnemySpawner() override;

    void Initialize() override;
    void Initialize(const Vector3& position);
    void Update() override;
    void Draw() override;

    void ShowGui() override;

    /* collision ======================================*/
    void OnCollisionEnter(Collider* other) override;
    void OnCollisionStay(Collider* other) override;
    void OnCollisionExit(Collider* other) override;

    // スポナーにターゲットを設定するメソッド（必要に応じて追加）
    void SetEnemyManager(EnemyManager* manager){ enemyManager_ = manager; }

private:
    int32_t life_ = 800;

    // スポーン間隔や経過時間の管理
    float spawnInterval_ = 8.0f;   // スポーン間隔（秒）
    float spawnTimer_ = 0.0f;       // 経過時間

    // EnemyManager への参照（敵をスポーンするために利用）
    EnemyManager* enemyManager_ = nullptr;
};
