#pragma once

#include "ParticleBehavior.h"

// lib
#include "lib/myMath/Vector3.h"
#include <cmath>

class PtlBehavior_Tornado :
    public ParticleBehavior{
public:
    //===================================================================*/
    //                   public methods
    //===================================================================*/
	PtlBehavior_Tornado() = default;
    PtlBehavior_Tornado(const Vector3& center, const float rotateSpeed,const float upSpeed);
    ~PtlBehavior_Tornado()override = default;

    void ApplyBehavior(ParticleSystem& system)override;

    void ShowImGui()override;

    void SaveJson(nlohmann::json& j)override;
    void LoadJson(nlohmann::json& j)override;

private:
    //===================================================================*/
    //                   private methods
    //===================================================================*/
    Vector3 center_;        // 旋回の中心
    float rotationSpeed_;   // y軸回転速度
    float upwardSpeed_;     // 上昇速度
    float radiusGrowthFactor_ = 0.015f;

};

