#include "PlayerAttackController.h"

#include "HorizonMowingDown.h"

#include "../Player.h"

#include <externals/imgui/imgui.h>

PlayerAttackController::PlayerAttackController(){
    // プレイヤーの攻撃を登録
    playerAttacks_["HorizonMowingDown"] = std::make_unique<HorizonMowingDown>("HorizonMowingDown");
}


void PlayerAttackController::Update(){
    for (auto& attack : playerAttacks_){
        attack.second->Update();
		attack.second->SetCenter(pPlayer_->GetCenterPos());
    }
}

void PlayerAttackController::ExecuteAttack(const std::string& attackName){
    auto it = playerAttacks_.find(attackName);
    if (it != playerAttacks_.end()){
        it->second->Execution();
    }
}

void PlayerAttackController::Draw(){
	for (auto& attack : playerAttacks_){
		attack.second->Draw();
	}
}

void PlayerAttackController::ShowGui(){
    for (auto& attack : playerAttacks_){
        if (ImGui::CollapsingHeader(attack.first.c_str())){
            if (ImGui::Button(("Execute " + attack.first).c_str())){
                attack.second->Execution();
            }
            attack.second->ShowGui();
        }
    }
}