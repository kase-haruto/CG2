#pragma once

#include "ComboData.h"
#include "IPlayerAttack.h"

#include <unordered_map>
#include <string>
#include <memory>
#include <vector>
#include <chrono>

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

	/* helper =====================================================*/
	// コンボの追加
	void AddCombo(const Combo& combo);
	// コンボ入力の処理
	void HandleComboInput(const std::string& attackName);

	void HandleAttackInput();                                    //< 攻撃入力の処理

private:
	//===================================================================*/
	//                   private member variables
	//===================================================================*/
	// 攻撃タイプの登録（テンプレート）
	std::unordered_map<std::string, std::unique_ptr<IPlayerAttack>> attackTemplates_;

	// 実行中の攻撃インスタンス
	std::vector<std::unique_ptr<IPlayerAttack>> activeAttacks_;

	const Player* pPlayer_ = nullptr;                        //< プレイヤー

	//定義されたコンボのリスト
	std::vector<Combo> combos_;

	// 現在のコンボのトラッキング
	struct ActiveCombo{
		const Combo* combo;
		size_t currentStep;
		std::chrono::steady_clock::time_point lastInputTime;
	};

	std::vector<ActiveCombo> activeCombos_;

	// コンボシーケンス
	std::vector<std::string> comboSequence_ = {
		"WeakDiagonalSlash",
		"HorizonMowingDown",
	};
	size_t currentComboStep_ = 0; // 現在のコンボステップ
	std::chrono::steady_clock::time_point lastAttackTime_; // 最後の攻撃時間
	const float comboTimeout_ = 1.0f; // コンボのタイムアウト時間（秒）

	// 次の攻撃の入力を保持
	std::optional<std::string> pendingAttack_ = std::nullopt;

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

	// 攻撃しているかどうか
	bool IsAttacking() const{
		return !activeAttacks_.empty();
	}

	// 攻撃テンプレートの取得
	std::unordered_map<std::string, std::unique_ptr<IPlayerAttack>>& GetAttackTemplates();

	// 特定の攻撃テンプレートを取得
	IPlayerAttack& GetAttackTemplate(const std::string& name);

	// 攻撃テンプレートの有無を確認
	bool HasAttackTemplate(const std::string& name) const;

};
