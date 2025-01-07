#pragma once

#include "../Base/Character.h"
#include "Engine/objects/Collider/BoxCollider.h"
#include "Engine/objects/particle/DemoParticle.h"
#include "Engine/physics/Shape.h"

#include "Engine/objects/particle/TornadoParticle.h"
#include "../Player/PlayerParticle/AttackParticle.h"
#include "../Player/PlayerParticle/HitParticle.h"

class Enemy : public Character, public BoxCollider{
public:
    //===================================================================*/
    //                   public methods
    //===================================================================*/
    Enemy() = default;
    Enemy(const std::string& modelName);
    ~Enemy() override;

    void Initialize() override;
    void Update() override;
    void Draw() override;

    /* collision ===================================*/
    void OnCollisionEnter(Collider* other) override;
    void OnCollisionStay(Collider* other) override;
    void OnCollisionExit(Collider* other) override;

    /* ui =========================================*/
    void ShowGui();

    void SetTarget(Vector3* target){ target_ = target; }

private:
    //===================================================================*/
    //                   private methods
    //===================================================================*/
    void Move();

private:
    //===================================================================*/
    //                   private members
    //===================================================================*/
    std::unique_ptr<AttackParticle> hitParticle_ = nullptr;
    std::unique_ptr<HitParticle> hitParticle2_ = nullptr;
    std::unique_ptr<TornadoParticle> deathParticle_ = nullptr;

    Vector3* target_ = nullptr;

    float movementSpeed_ = 2.0f; // 移動速度（ユニット/秒）

    float deathAnimationTimer_ = 0.0f;
    float deathAnimationDuration_ = 1.5f;
};
