#include "Character.h"

#include "lib/myFunc/MyFunc.h"
#include "Engine/core/Json/JsonCoordinator.h"

Character::Character(const std::string& modelName)
:BaseGameObject(modelName){}

void Character::Initialize(){

	JsonCoordinator::RegisterItem(name_, "MoveSpeed", moveSpeed_);

}

void Character::Update(){

    const float gravity = -9.8f;             // 重力加速度
    const float terminalVelocity = -50.0f;  // 最大落下速度

    // 重力による加速度を計算
    acceleration_ = {0.0f, gravity, 0.0f};

    // 加速度を考慮して速度を更新（経過時間を考慮）
    velocity_ += acceleration_ * deltaTime;

    // 落下速度を最大落下速度に制限
    if (velocity_.y < terminalVelocity){
        velocity_.y = terminalVelocity;
    }

    // 地面に接触しているかどうかを判定
    if (model_->transform.translate.y <= 0.0f){
        onGround_ = true;
        model_->transform.translate.y = 0.0f; // 地面上に位置を固定
    } else{
        onGround_ = false;
    }

    // 空中にいる場合の移動処理
    if (!onGround_){
        model_->transform.translate += velocity_ * deltaTime;
    } else{
        // 接地している場合、Y方向の速度をリセット
        velocity_.y = 0.0f;
    }

    // キャラクターの生存フラグを更新
    if (life_ <= 0){
        isAlive_ = false;
    }
}


void Character::ShowGui(){

	JsonCoordinator::RenderGroupUI(name_);

}
