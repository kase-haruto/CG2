#include "ClockManager.h"

// lib
#include <algorithm>

ClockManager::ClockManager(){
    firstFrameTime_ = std::chrono::high_resolution_clock::now();
    lastFrameTime_ = firstFrameTime_;
}

void ClockManager::Update(){
    auto currentFrameTime = std::chrono::high_resolution_clock::now();

    // deltaTime 計算
    float dt = std::chrono::duration<float, std::milli>(currentFrameTime - lastFrameTime_).count() / 1000.0f;
    dt = std::clamp(dt, 0.0f, 0.017f);

    // FPS 計算
    if (dt > 0.0f){
        currentFPS_ = 1.0f / dt;
    }

    // ヒットストップ処理
    if (isHitStopActive_){
        hitStopElapsed_ += dt;
        float progress = hitStopElapsed_ / hitStopDuration_;

        // 減速フェーズ (前半)
        if (progress < 0.5f){
            float t = progress / 0.5f;
            currentTimeScale_ = normalTimeScale_ * easeOutCurve_(t);
        }
        // 加速フェーズ (後半)
        else{
            float t = (progress - 0.5f) / 0.5f;
            currentTimeScale_ = normalTimeScale_ * easeInCurve_(t);
        }

        if (hitStopElapsed_ >= hitStopDuration_){
            isHitStopActive_ = false;
            currentTimeScale_ = normalTimeScale_;
        }
    } else{
        currentTimeScale_ = normalTimeScale_;
    }

    currentDeltaTime_ = dt * currentTimeScale_;

    totalTime_ = std::chrono::duration<float>(currentFrameTime - firstFrameTime_).count();

    frameCount_++;
    if (totalTime_ > 0.0f){
        averageFPS_ = static_cast< float >(frameCount_) / totalTime_;
    }

    lastFrameTime_ = currentFrameTime;
}

void ClockManager::StartHitStop(float duration, std::function<float(float)> easeOutCurve, std::function<float(float)> easeInCurve){
    isHitStopActive_ = true;
    hitStopDuration_ = duration;
    hitStopElapsed_ = 0.0f;
    easeOutCurve_ = easeOutCurve;
    easeInCurve_ = easeInCurve;
}
