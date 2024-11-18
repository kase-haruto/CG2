#pragma once

#include "engine/objects/Transform.h"
#include "lib/myMath/Vector3.h"
#include "lib/myMath/Vector4.h"

#include <unordered_map>
#include <string>
/// <summary>
/// パーティクル構造体
/// </summary>
struct Particle{
    Transform transform;   // パーティクルの位置・回転・スケール
    Vector3 velocity;      // 速度
    Vector4 color;         // 色 (RGBA)
    float lifeTime;        // パーティクルの寿命
    float currentTime;     // 現在の生存時間

    std::unordered_map<std::string, float> attributes; // 任意の属性を管理するマップ
};
