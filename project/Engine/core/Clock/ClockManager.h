#pragma once
#include <chrono>
#include <functional>

class ClockManager{
public:
    // シングルトン取得メソッド
    static ClockManager* GetInstance(){
        static ClockManager instance;  // 唯一のインスタンス
        return &instance;
    }

    // コピーコンストラクタと代入演算子を削除
    ClockManager(const ClockManager&) = delete;
    void operator=(const ClockManager&) = delete;

    void Update();  // 毎フレーム呼び出し
    float GetDeltaTime() const{ return currentDeltaTime_; }
    float GetTotalTime() const{ return totalTime_; }
    float GetCurrentFPS() const{ return currentFPS_; }
    float GetAverageFPS() const{ return averageFPS_; }

    // ヒットストップ開始
    void StartHitStop(float duration, std::function<float(float)> easeOutCurve, std::function<float(float)> easeInCurve);

private:
    // コンストラクタをprivate化
    ClockManager();

    std::chrono::high_resolution_clock::time_point firstFrameTime_;
    std::chrono::high_resolution_clock::time_point lastFrameTime_;

    float currentDeltaTime_ = 0.016f;
    float totalTime_ = 0.0f;
    float currentFPS_ = 60.0f;
    float averageFPS_ = 60.0f;
    unsigned long long frameCount_ = 0;

    // ヒットストップ関連
    bool isHitStopActive_ = false;
    float hitStopDuration_ = 0.0f;
    float hitStopElapsed_ = 0.0f;
    float normalTimeScale_ = 1.0f;
    float currentTimeScale_ = 1.0f;

    std::function<float(float)> easeOutCurve_;
    std::function<float(float)> easeInCurve_;
};
