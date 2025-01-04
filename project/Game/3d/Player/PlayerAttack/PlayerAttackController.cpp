#include "PlayerAttackController.h"

#include "HorizonMowingDown.h"
#include "WeakDiagonalSlash.h"

#include "../Player.h"

#include <externals/imgui/imgui.h>
#include <algorithm>

PlayerAttackController::PlayerAttackController(){
    // 初期攻撃テンプレートの登録
    attackTemplates_["HorizonMowingDown"] = std::make_unique<HorizonMowingDown>("HorizonMowingDown");
    attackTemplates_["WeakDiagonalSlash"] = std::make_unique<WeakDiagonalSlash>("WeakDiagonalSlash");
}

void PlayerAttackController::Initialize(){
    for (auto& [name, attackTemplate] : attackTemplates_){
        attackTemplate->Initialize();
    }
}

void PlayerAttackController::Update(){
    // 実行中の攻撃を更新
    for (auto& attack : activeAttacks_){
        attack->Update();
        attack->SetCenter(pPlayer_->GetCenterPos()); // プレイヤーの位置を設定
    }

    // 終了した攻撃を削除
    activeAttacks_.erase(
        std::remove_if(activeAttacks_.begin(), activeAttacks_.end(),
        [&] (const std::unique_ptr<IPlayerAttack>& attack) -> bool{
            return !attack->IsAttacking();
        }),
        activeAttacks_.end()
    );
}

void PlayerAttackController::ExecuteAttack(const std::string& attackName){
    auto it = attackTemplates_.find(attackName);
    if (it != attackTemplates_.end()){
        // テンプレートから新しい攻撃インスタンスを生成
        std::unique_ptr<IPlayerAttack> newAttack = it->second->Clone();
        newAttack->SetCenter(pPlayer_->GetCenterPos());
        newAttack->Execution();
		newAttack->SetWeapon(pPlayer_->GetWeapon());
        activeAttacks_.emplace_back(std::move(newAttack));
    }
}

void PlayerAttackController::Draw(){
    for (auto& attack : activeAttacks_){
        attack->Draw();
    }
}

void PlayerAttackController::ShowGui(){
    ImGui::Begin("Player Attack Controller");

    // 攻撃タイプの表示と実行ボタン
    ImGui::Text("Attack Types:");
    for (const auto& [attackName, attackTemplate] : attackTemplates_){
        if (ImGui::Button(("Execute " + attackName).c_str())){
            ExecuteAttack(attackName);
        }
    }

    ImGui::Separator();

    // 実行中の攻撃インスタンスの表示
    ImGui::Text("Active Attacks:");
    for (size_t i = 0; i < activeAttacks_.size(); ++i){
        auto& attack = activeAttacks_[i];
        std::string header = "Attack Instance " + std::to_string(i) + ": " + attack->GetName();
        if (ImGui::CollapsingHeader(header.c_str())){
            attack->ShowGui();
        }
    }

    ImGui::End();
}




std::unordered_map<std::string, std::unique_ptr<IPlayerAttack>>& PlayerAttackController::GetAttackTemplates(){
    return attackTemplates_;
}

IPlayerAttack& PlayerAttackController::GetAttackTemplate(const std::string& name){
    return *(attackTemplates_.at(name));
}

bool PlayerAttackController::HasAttackTemplate(const std::string& name) const{
    return attackTemplates_.find(name) != attackTemplates_.end();
}