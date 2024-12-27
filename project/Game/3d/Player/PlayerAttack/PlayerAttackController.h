#pragma once

#include "IPlayerAttack.h"

#include <unordered_map>
#include <string>
#include <memory>

class Player;

class PlayerAttackController{
public:
    //===================================================================*/
    //                   public member functions
    //===================================================================*/
    PlayerAttackController();
    ~PlayerAttackController() = default;

    void Initialize();                                  //< 初期化
    void Update();                                      //< 更新
    void ExecuteAttack(const std::string& attackName);  //< 攻撃を実行
	void Draw();                                        //< 描画
    void ShowGui();                                     //< GUI表示

private:
    //===================================================================*/
    //                   private member variables
    //===================================================================*/
    std::unordered_map<std::string, std::unique_ptr<IPlayerAttack>> playerAttacks_; //< プレイヤーの攻撃
	const Player* pPlayer_ = nullptr;                                                //< プレイヤー

public:
	//===================================================================*/
	//                   getter
	//===================================================================*/
	void SetPlayer(const Player* player){ pPlayer_ = player; }
};


