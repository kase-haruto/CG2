#include "PlayerAttackController.h"

#include "HorizonMowingDown.h"
#include "WeakDiagonalSlash.h"
#include "DiagonalSlashRightToLeft.h"
#include "JumpAttack.h"

#include "../Player.h"

#include <externals/imgui/imgui.h>
#include <algorithm>

PlayerAttackController::PlayerAttackController(){
	// 初期攻撃テンプレートの登録
	 // テンプレートは非アクティブな状態で作成
	auto horizonAttack = std::make_unique<HorizonMowingDown>("HorizonMowingDown");
	horizonAttack->SetIsActive(false); // テンプレート自体は非アクティブ
	attackTemplates_["HorizonMowingDown"] = std::move(horizonAttack);

	auto weakSlashAttack = std::make_unique<WeakDiagonalSlash>("WeakDiagonalSlash");
	weakSlashAttack->SetIsActive(false); // テンプレート自体は非アクティブ
	attackTemplates_["WeakDiagonalSlash"] = std::move(weakSlashAttack);

	auto rightToLeftSlash = std::make_unique<DiagonalSlashRightToLeft>("DiagonalSlashRightToLeft");
	rightToLeftSlash->SetIsActive(false); // テンプレート自体は非アクティブ
	attackTemplates_["DiagonalSlashRightToLeft"] = std::move(rightToLeftSlash);

	auto jumpAttack = std::make_unique<JumpAttack>("JumpAttack");
	jumpAttack->SetIsActive(false); // テンプレート自体は非アクティブ
	attackTemplates_["JumpAttack"] = std::move(jumpAttack);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//                      main functions
////////////////////////////////////////////////////////////////////////////////////////////////////
void PlayerAttackController::Initialize(){
	for (auto& [name, attackTemplate] : attackTemplates_){
		attackTemplate->Initialize();
	}

	// コンボの定義例
	Combo combo1;
	combo1.name = "StrongCombo";
	combo1.sequence = {
	 {"WeakDiagonalSlash", 0.5f},  // 正しい
	 {"HorizonMowingDown", 0.5f},  // 正しい
	 {"DiagonalSlashRightToLeft", 0.5f},  // 正しい
	 {"JumpAttack", 0.5f},          // 正しい
	};
	AddCombo(combo1);
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

	// すべての攻撃が終了したら、トレイルをクリアする
	if (activeAttacks_.empty()){
		pPlayer_->GetWeapon()->ClearTrail();
		pPlayer_->GetWeapon()->SetComboActive(false);
	}

	// コンボのタイムアウトチェック
	auto now = std::chrono::steady_clock::now();
	if (currentComboStep_ > 0){
		float elapsed = std::chrono::duration<float>(now - lastAttackTime_).count();
		if (elapsed > comboTimeout_){
			// タイムアウトした場合、コンボをリセット
			currentComboStep_ = 0;
			pendingAttack_ = std::nullopt;
			pPlayer_->GetWeapon()->ClearTrail();  // 🔥 コンボの時間切れでもトレイルをクリア
		}
	}

	// 攻撃が終了していて、次の攻撃がキューにある場合
	if (activeAttacks_.empty() && pendingAttack_.has_value()){
		ExecuteAttack(pendingAttack_.value());
		currentComboStep_++;
		lastAttackTime_ = now;
		pendingAttack_ = std::nullopt;
	}
}


void PlayerAttackController::ExecuteAttack(const std::string& attackName){
	auto it = attackTemplates_.find(attackName);
	if (it != attackTemplates_.end()){
		std::unique_ptr<IPlayerAttack> newAttack = it->second->Clone();
		newAttack->SetCenter(pPlayer_->GetCenterPos());
		newAttack->SetPlayer(const_cast< Player* >(pPlayer_));
		newAttack->SetWeapon(pPlayer_->GetWeapon());
		newAttack->Execution();
		activeAttacks_.emplace_back(std::move(newAttack));

		// コンボが開始されたら Weapon に通知
		pPlayer_->GetWeapon()->SetComboActive(true);
	}
}

void PlayerAttackController::Draw(){
	for (auto& attack : activeAttacks_){
		attack->Draw();
	}
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//                    helper functions
////////////////////////////////////////////////////////////////////////////////////////////////////
// コンボの追加
void PlayerAttackController::AddCombo(const Combo& combo){
	combos_.emplace_back(combo);
}

// コンボ入力の処理
void PlayerAttackController::HandleComboInput(const std::string& attackName){
	auto now = std::chrono::steady_clock::now();
	std::vector<ActiveCombo> combosToStart;

	// 既存のアクティブコンボを更新
	for (auto it = activeCombos_.begin(); it != activeCombos_.end(); ){
		auto& activeCombo = *it;
		const ComboInput& expectedInput = activeCombo.combo->sequence[activeCombo.currentStep];
		float elapsed = std::chrono::duration<float>(now - activeCombo.lastInputTime).count();

		if (elapsed > expectedInput.maxInterval){
			// タイムアウトしたコンボを削除
			it = activeCombos_.erase(it);
			continue;
		}

		if (attackName == expectedInput.attackName){
			// 次のステップに進む
			activeCombo.currentStep++;
			activeCombo.lastInputTime = now;

			if (activeCombo.currentStep >= activeCombo.combo->sequence.size()){
				// コンボ完了
				// コンボの攻撃を順に実行
				for (const auto& comboAttack : activeCombo.combo->sequence){
					ExecuteAttack(comboAttack.attackName);
				}
				it = activeCombos_.erase(it);
				continue;
			}
		}

		++it;
	}

	// 新しいコンボの開始をチェック
	for (const auto& combo : combos_){
		if (!combo.sequence.empty() && combo.sequence[0].attackName == attackName){
			ActiveCombo newActiveCombo;
			newActiveCombo.combo = &combo;
			newActiveCombo.currentStep = 1;
			newActiveCombo.lastInputTime = now;
			activeCombos_.emplace_back(newActiveCombo);
		}
	}
}

// 攻撃入力の処理
void PlayerAttackController::HandleAttackInput(){
	auto now = std::chrono::steady_clock::now();
	float elapsed = std::chrono::duration<float>(now - lastAttackTime_).count();

	if (elapsed > comboTimeout_){
		// タイムアウトした場合、コンボをリセット
		currentComboStep_ = 0;
		pendingAttack_ = std::nullopt;
	}

	if (currentComboStep_ < comboSequence_.size()){
		if (activeAttacks_.empty()){
			// 攻撃が実行中でなければ、次の攻撃を即座に実行
			ExecuteAttack(comboSequence_[currentComboStep_]);
			currentComboStep_++;
			lastAttackTime_ = now;
		} else{
			// 攻撃が実行中の場合、次の攻撃をキューに保持
			pendingAttack_ = comboSequence_[currentComboStep_];
		}
	} else{
		// コンボの最後まで達した場合、リセット
		currentComboStep_ = 0;
		pendingAttack_ = std::nullopt;
	}
}

// 攻撃テンプレートの取得
std::unordered_map<std::string, std::unique_ptr<IPlayerAttack>>& PlayerAttackController::GetAttackTemplates(){
	return attackTemplates_;
}

// 特定の攻撃テンプレートを取得
IPlayerAttack& PlayerAttackController::GetAttackTemplate(const std::string& name){
	return *(attackTemplates_.at(name));
}

// 攻撃テンプレートの追加
bool PlayerAttackController::HasAttackTemplate(const std::string& name) const{
	return attackTemplates_.find(name) != attackTemplates_.end();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//                      ui/gui
////////////////////////////////////////////////////////////////////////////////////////////////////
void PlayerAttackController::ShowGui(){
	ImGui::Begin("Player Attack Controller");

	// 攻撃タイプの表示と実行ボタン（必要に応じて）
	ImGui::Text("Attack Types:");
	for (const auto& [attackName, attackTemplate] : attackTemplates_){
		if (ImGui::Button(("Execute " + attackName).c_str())){
			ExecuteAttack(attackName);
		}
	}

	ImGui::Separator();

	// 現在のコンボステップの表示
	ImGui::Text("Current Combo Step: %zu", currentComboStep_);
	if (currentComboStep_ > 0 && currentComboStep_ <= comboSequence_.size()){
		ImGui::Text("Next Attack: %s", comboSequence_[currentComboStep_ - 1].c_str());
	}

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