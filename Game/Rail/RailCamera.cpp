#include "Rail/RailCamera.h"

#include <cmath>
#include "myFunc/MyFunc.h"
#include "imgui.h"

#include <algorithm>
#undef max

void RailCamera::Initialize(){
    transform_.scale = {1.0f, 1.0f, 1.0f};
    transform_.rotate = {0.0f, 0.0f, 0.0f};
    transform_.translate = {0.0f, 2.0f, 0.0f};
    t_ = 0.0f;
    isPaused_ = false;
    pauseTimer_ = 0.0f;

    Update();
}

void RailCamera::Update(){

    // 停止状態の処理
    if (isPaused_){
        pauseTimer_ += deltaTime;
        if (pauseTimer_ >= pauseDuration_){
            isPaused_ = false;      // 停止終了
            pauseTimer_ = 0.0f;     // タイマーリセット
            pauseComplete_ = true;  // 停止完了をマーク
        }
        return;  // 停止中はこれ以上更新しない
    }

    // `t_`が0.85に達し、一度も停止していない場合のみ停止を開始
    if (t_ >= 0.86f && !isPaused_ && !pauseComplete_){
        isPaused_ = true;          // 停止状態に設定
        pauseTimer_ = 0.0f;        // タイマーリセット
        return;
    }

    // 現在の`t_`での位置と次の位置を取得
    Vector3 currentPos = CatmullRomPosition(ctrlPoints_, t_);
    float t_next = std::clamp(t_ + 0.01f, 0.0f, 1.0f); // 次の位置を少し先に設定
    Vector3 nextPos = CatmullRomPosition(ctrlPoints_, t_next);

    // 高さの差分を計算して傾斜を得る
    float heightDifference = nextPos.y - currentPos.y;
    float slopeFactor = 0.0001f; // 傾斜に応じた速度変化の係数（調整可能）

    // 傾斜に応じた速度調整：上り坂で減速、下り坂で加速
    float baseSpeed = 0.0002f;
    float adjustedSpeed = baseSpeed - heightDifference * slopeFactor; // 符号を反転

    // 最小速度を設定
    adjustedSpeed = std::max(adjustedSpeed, 0.0001f);

    // `t_`を調整後の速度で増加
    t_ += adjustedSpeed;

    // `t_`が1.0を超えたら0.0に戻してループ
    if (t_ > 1.0f){
        t_ -= 1.0f;
        pauseComplete_ = false;  // ループ後に再び停止可能にする
        isFinishedRail_ = true;
    }

    // カメラの位置と注視点の更新
    Vector3 eye = CatmullRomPosition(ctrlPoints_, t_);

    float t_2 = t_ + 0.02f;
    t_2 = std::clamp(t_2, 0.0f, 1.0f);

    Vector3 target = CatmullRomPosition(ctrlPoints_, t_2);
    Vector3 forward = (target - eye).Normalize();

    // トランスフォームの更新
    transform_.translate = {eye.x, eye.y + 2.0f, eye.z};
    float horizontalDistance = sqrtf(forward.x * forward.x + forward.z * forward.z);
    transform_.rotate.x = std::atan2(-forward.y, horizontalDistance);
    transform_.rotate.y = std::atan2(forward.x, forward.z);
}