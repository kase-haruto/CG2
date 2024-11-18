#include "engine/objects/ParticleSystem/Emitter.h"

// コンストラクタ
Emitter::Emitter()
    : transform(), count(0), frequency(1.0f), elapsedTime(0.0f){}

// 初期化
void Emitter::Initialize(const Transform& Transform, uint32_t count, float frequency){
    transform = Transform;
    count = count;
    frequency = frequency;
    elapsedTime = 0.0f;
}

// 更新
void Emitter::Update(float deltaTime){
    elapsedTime += deltaTime;
}

// 発生可能か確認
bool Emitter::CanEmit() const{
    return elapsedTime >= frequency;
}

// 発生頻度の設定
void Emitter::SetFrequency(float frequency){
    frequency = frequency;
}

// 発生数の設定
void Emitter::SetCount(uint32_t count){
    count = count;
}

// トランスフォームの設定
void Emitter::SetTransform(const Transform& Transform){
    transform = Transform;
}
