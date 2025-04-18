#include "PtlBehavior_Tornado.h"
#include "Engine/core/System.h"
#include "../Particle.h"
#include "Engine/core/Clock/ClockManager.h"
#include "lib/myFunc/MyFunc.h"

#include <externals/imgui/imgui.h>

PtlBehavior_Tornado::PtlBehavior_Tornado(const Vector3& center, const float rotateSpeed, const float upSpeed)
:center_(center),rotationSpeed_(rotateSpeed),upwardSpeed_(upSpeed){}

void PtlBehavior_Tornado::ApplyBehavior(Particle& system){
    center_ = system.GetEmitterPos();
    auto& particles = system.particles_;

    for (auto& p : particles){
        Vector3 relativePos = p.transform.translate - center_;

        // Y軸回転
        float angle = rotationSpeed_ * ClockManager::GetInstance()->GetDeltaTime();
        float cosA = std::cos(angle);
        float sinA = std::sin(angle);

        float newX = relativePos.x * cosA - relativePos.z * sinA;
        float newZ = relativePos.x * sinA + relativePos.z * cosA;
        relativePos.x = newX;
        relativePos.z = newZ;

        // 半径拡大スケールを計算
        float lifeRatio = p.currentTime / p.lifeTime;           // 0.0〜1.0
        float radiusScale = 1.0f + lifeRatio * radiusGrowthFactor_; // 成長率を掛ける

        relativePos *= radiusScale;

        // 座標を戻す
        p.transform.translate = center_ + relativePos;

        // 上昇
        p.transform.translate.y += upwardSpeed_ * ClockManager::GetInstance()->GetDeltaTime();
    }
}

void PtlBehavior_Tornado::ShowImGui(){

    ImGui::DragFloat("rotateSpeed", &rotationSpeed_, 0.01f);
    ImGui::DragFloat("upSpeed", &upwardSpeed_, 0.01f);
    ImGui::DragFloat("radiusGrowthFactor", &radiusGrowthFactor_, 0.001f);

}

void PtlBehavior_Tornado::SaveJson(nlohmann::json& j){

        j["rotateSpeed"] = rotationSpeed_;
        j["upSpeed"] = upwardSpeed_;
        j["radiusGrowthFactor"] = radiusGrowthFactor_;

        // centerはベクターとして保存
        j["center"] = {center_.x, center_.y, center_.z};

}

void PtlBehavior_Tornado::LoadJson(nlohmann::json& j){
    if (j.contains("rotateSpeed")){
        rotationSpeed_ = j["rotateSpeed"].get<float>();
    }
    if (j.contains("upSpeed")){
        upwardSpeed_ = j["upSpeed"].get<float>();
    }
    if (j.contains("radiusGrowthFactor")){
        radiusGrowthFactor_ = j["radiusGrowthFactor"].get<float>();
    }
}

