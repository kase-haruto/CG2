// PlayerAttackEditor.h
#pragma once

#include <string>
#include <vector>
#include <memory>
#include "lib/myMath/Vector3.h"
#include "PlayerAttackController.h"

class PlayerAttackEditor{
public:
    //===================================================================*/
    //                   public member functions
    //===================================================================*/
    PlayerAttackEditor(PlayerAttackController* controller);
    ~PlayerAttackEditor() = default;

    void ShowGui(bool isShow); // GUI表示

private:
    char newAttackNameBuffer_[256]; // 新規攻撃名入力用のバッファ
    PlayerAttackController* attackController_; // 編集対象のAttackController

    // 選択中の攻撃タイプ
    std::string selectedAttackName_;

    // ユーティリティ関数
    void EditControlPoints(std::unique_ptr<IPlayerAttack>& attack);
};
