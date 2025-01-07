#pragma once
#include "Engine/objects/Collider/SphereCollider.h"
#include "Game/3d/BaseGameObject.h"
#include "../Player/PlayerParticle/AttackParticle.h"
#include "../Player/PlayerParticle/HitParticle.h"
#include "Engine/objects/Sprite.h"

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

	void DrawUI();

    void ShowGui() override;

    /* collision ======================================*/
    void OnCollisionEnter(Collider* other) override;
    void OnCollisionStay(Collider* other) override;
    void OnCollisionExit(Collider* other) override;

    // スポナーにターゲットを設定するメソッド（必要に応じて追加）
    void SetEnemyManager(EnemyManager* manager){ enemyManager_ = manager; }

	bool IsActive() const{ return isActive_; }
private:
    int32_t life_ = 800;
	const int32_t maxLife_ = 800;

    // スポーン間隔や経過時間の管理
    float spawnInterval_ = 10.0f;   // スポーン間隔（秒）
    float spawnTimer_ = 0.0f;       // 経過時間
	bool isActive_ = true;          // スポナーがアクティブかどうか

    // EnemyManager への参照（敵をスポーンするために利用）
    EnemyManager* enemyManager_ = nullptr;

private:
    //パーツ
	std::unique_ptr<Model> core_ = nullptr;
	std::unique_ptr<Model> head_ = nullptr;

    std::unique_ptr<HitParticle> hitParticle_ = nullptr;

	float floatingPhaseOffset_ = 0.0f; // 浮遊動作のフェーズオフセット
	int32_t rotateDir_ = 1;            // 回転方向（1: 正回転, -1: 逆回転）
	float rotateTimer_ = 0.0f;         // 回転用タイマー


    // 非アクティブ時のトランジション管理用
    bool isTransitioning_ = false;
    bool rotationDone_ = false;       // 回転完了フラグ
    float transitionTimer_ = 0.0f;

    // 回転と落下の各期間（秒）
    const float rotationDuration_ = 1.5f;  // 回転にかける時間
    const float fallDuration_ = 1.5f;      // 落下にかける時間

    float initialY_ = 0.0f;
    float initialRotY_ = 0.0f;
    const float fallDistance_ = 5.0f;  // 下方向に移動する距離

	std::unique_ptr<Sprite> hp_ = nullptr;
    
};
