#pragma once

#include "IPlayerAttack.h"

#include <unordered_map>
#include <string>
#include <memory>
#include <vector>

class Player;

// 攻撃管理クラス
class PlayerAttackController{
public:
    //===================================================================*/
    //                   public member functions
    //===================================================================*/
    PlayerAttackController();
    ~PlayerAttackController() = default;

    void Initialize();                                       //< 初期化
    void Update();                                           //< 更新
    void ExecuteAttack(const std::string& attackName);      //< 攻撃を実行
    void Draw();                                             //< 描画
    void ShowGui();                                          //< GUI表示

private:
    //===================================================================*/
    //                   private member variables
    //===================================================================*/
    // 攻撃タイプの登録（テンプレート）
    std::unordered_map<std::string, std::unique_ptr<IPlayerAttack>> attackTemplates_;

    // 実行中の攻撃インスタンス
    std::vector<std::unique_ptr<IPlayerAttack>> activeAttacks_;

    const Player* pPlayer_ = nullptr;                        //< プレイヤー

public:
    //===================================================================*/
    //                   getter/setter
    //===================================================================*/
    void SetPlayer(const Player* player){ pPlayer_ = player; }

    // 攻撃テンプレートへのアクセス
    const std::unordered_map<std::string, std::unique_ptr<IPlayerAttack>>& GetAttackTemplates() const{
        return attackTemplates_;
    }

    // 攻撃テンプレートの追加
    void AddAttackTemplate(const std::string& attackName, std::unique_ptr<IPlayerAttack> attackTemplate){
        attackTemplates_[attackName] = std::move(attackTemplate);
    }

    // 攻撃テンプレートの削除
    void RemoveAttackTemplate(const std::string& attackName){
        attackTemplates_.erase(attackName);
    }
};
