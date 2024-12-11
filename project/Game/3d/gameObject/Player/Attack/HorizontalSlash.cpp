#include "HorizontalSlash.h"
#include "Game/3d/gameObject/Player/Weapon.h"
#include "../Player.h"
#include "Engine/collision/CollisionManager.h"
#include "lib/myFunc/MyFunc.h"

#include <externals/imgui/imgui.h>
#include <numbers>

HorizontalSlash::HorizontalSlash(){

	BoxCollider::Initialize(attackRange_.size);
	BoxCollider::SetName("horizontalSlash");
      
    shape_.size.z = 3.0f;

}


void HorizontalSlash::Initialize(){
	startPos_ = pPlayer_->GetForward(0.5f);
	attackRange_.position = startPos_;
    isAttacking_ = true;
}

void HorizontalSlash::Execution(){
    
    // 回転の速度を設定（ラジアン/フレーム）
    constexpr float rotationSpeed = 8.0f; // 回転速度（例: 1秒で1回転なら 2 * PI / フレーム数）

    // プレイヤーの中心位置を取得
    Vector3 center = pPlayer_->GetCenterPos();

    // プレイヤー中心から攻撃範囲までの相対距離を計算
    Vector3 relativePos = attackRange_.position - center;

    // 半径を制限（必要に応じて）
    float radius = relativePos.Length(); // 現在の半径を計算
    constexpr float maxRadius = 3.0f;   // 最大半径（適切な値に設定）
    if (radius > maxRadius){
        relativePos = relativePos.Normalize() * maxRadius; // 半径を制限
    }

    // 回転行列を使用して新しい相対座標を計算（XZ平面での回転）
    float cosAngle = std::cos(rotationSpeed * deltaTime); // deltaTimeを適用
    float sinAngle = std::sin(rotationSpeed * deltaTime);
    float rotatedX = relativePos.x * cosAngle - relativePos.z * sinAngle;
    float rotatedZ = relativePos.x * sinAngle + relativePos.z * cosAngle;

    // 攻撃範囲の新しい位置を計算
    attackRange_.position = center + Vector3(rotatedX, relativePos.y, rotatedZ);

    // 判定用の座標を更新
    shape_.center = attackRange_.position;

    // 現在の回転方向に基づき `shape_` の回転を更新
    shape_.rotate = Vector3(0.0f, std::atan2(rotatedX, rotatedZ), 0.0f);

    // 回転の合計角度を更新
    currentRotationAngle_ += rotationSpeed * deltaTime;

   

    // 一回転（2πラジアン）を超えたら終了
    if (currentRotationAngle_ >= 2.0f * float(std::numbers::pi)){
        isAttacking_ = false;          // 攻撃終了
        currentRotationAngle_ = 0.0f; // 次の攻撃に備えてリセット
    }

}





void HorizontalSlash::Draw(){



	IPlayerAttack::Draw();

}