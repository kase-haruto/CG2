#pragma once 

/* engine */
#include "graphics/ViewProjection.h"
#include "objects/Enemy.h"

/* lib */
#include <list>
#include <memory>

class EnemyManager { 
public: 
    EnemyManager(); 
    ~EnemyManager(); 

    //初期化
    void Initialize();

    //更新
    void Update();

    //リストの描画
    void Draw();

    //リストに敵を追加
    void AddEnemy(const Vector3& pos);

    //リストの初期化
    void Reset();

    // enemies_のゲッター
    const std::list<std::unique_ptr<Enemy>>& GetEnemies() const{
        return enemies_;
    }

public:
    void SetViewProjection(const ViewProjection* viewProjection){ pViewProjection_ = viewProjection; }

private: 
    const ViewProjection* pViewProjection_ = nullptr;

    //* 敵のリスト
    std::list<std::unique_ptr<Enemy>> enemies_ {};
};