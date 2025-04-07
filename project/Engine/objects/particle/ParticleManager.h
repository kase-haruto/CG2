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

    void Draw();
    void Finalize();
    void ShowDebugUI();

    //--------- particleの追加・削除 -----------------------------------------------------
    void AddSystem(ParticleSystem* system);
    void RemoveSystem(ParticleSystem* system);

    //--------- accessor -----------------------------------------------------
	std::vector<ParticleSystem*>& GetParticleSystems(){ return systems_; }

private:
    //===================================================================*/
    //                   private Methods
    //===================================================================*/

    ParticleManager();
    ~ParticleManager() = default;


private:
    std::vector<ParticleSystem*> systems_; // 管理する粒子システム
};