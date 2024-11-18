#pragma once

#include "engine/objects/Transform.h"
#include <cstdint>

/// <summary>
/// エミッタークラス
/// パーティクルを生成する元となる設定を管理
/// </summary>
class Emitter{
public:
    // コンストラクタ
    Emitter();

    // 初期化
    void Initialize(const Transform& transform, uint32_t count, float frequency);

    // 更新
    void Update(float deltaTime);

    // エミッターの設定
    void SetFrequency(float frequency);
    void SetCount(uint32_t count);
    void SetTransform(const Transform& transform);

    // 現在の状態を取得
    bool CanEmit() const;

public:
    Transform transform;    // エミッタのトランスフォーム
    uint32_t count;         // 発生するパーティクル数
    float frequency;        // 発生頻度（秒）
    float elapsedTime;      // 経過時間
};
