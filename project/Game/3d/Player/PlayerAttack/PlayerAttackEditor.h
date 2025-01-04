// PlayerAttackEditor.h
#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include "lib/myMath/Vector3.h"
#include "lib/myMath/Quaternion.h" // Quaternionクラスのインクルード
#include "PlayerAttackController.h"
#include "Game/3d/Player/Weapon/Weapon.h"

class Player;

class PlayerAttackEditor{
public:
    //===================================================================*/
    //                   public member functions
    //===================================================================*/
    PlayerAttackEditor(PlayerAttackController* controller);
    ~PlayerAttackEditor() = default;

    void ShowGui(); // GUI表示

    void Draw();

    // 保存・読み込み機能
    void SaveAttackTemplates(const std::string& filepath);
    void LoadAttackTemplates(const std::string& filepath);

    //===================================================================*/
    //                   getter/setter
    //===================================================================*/
    void SetPlayer(Player* player){
        pPlayer_ = player;
    }

private:
    char newAttackNameBuffer_[256]; // 新規攻撃名入力用のバッファ
    PlayerAttackController* attackController_; // 編集対象のAttackController
    Player* pPlayer_; // プレイヤーオブジェクトへの参照

    // 選択中の攻撃タイプ
    std::string selectedAttackName_;

    // スプラインキャッシュ
    struct SplineCache{
        std::vector<Vector3> points;
        bool dirty = true; // 制御点が変更されたかどうか
    };
    std::unordered_map<std::string, SplineCache> splineCaches_;

    // ラインスタイルの設定
    struct LineStyle{
        float color[4] = {1.0f, 1.0f, 1.0f, 1.0f}; // RGBA
        float thickness = 2.0f;
    };
    LineStyle lineStyle_;

    // ユーティリティ関数
    void EditControlPoints(std::unique_ptr<IPlayerAttack>& attack);

    // 座標変換用の補助関数
    Vector3 ApplyWeaponTransform(const Vector3& localPos, const Vector3& weaponPos, const Quaternion& weaponRot);
};
