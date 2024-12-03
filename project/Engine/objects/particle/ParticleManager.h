#pragma once

#include <vector>
#include <memory>
#include "ParticleSystem.h"

class ParticleManager{
public:
    //===================================================================*/
    //                   singleton
    //===================================================================*/
    static ParticleManager* GetInstance();

    // コピーコンストラクタと代入演算子を削除
    ParticleManager(const ParticleManager&) = delete;
    ParticleManager& operator=(const ParticleManager&) = delete;

public:
    //===================================================================*/
    //                   public Methods
    //===================================================================*/

    // 粒子システムを追加
    void AddSystem(ParticleSystem* system);

    // 描画処理
    void Draw();

    void Finalize();

private:   
    //===================================================================*/
    //                   private Methods
    //===================================================================*/

    ParticleManager();
    ~ParticleManager() = default;


private:
    std::vector<ParticleSystem*> systems_; // 管理する粒子システム
};