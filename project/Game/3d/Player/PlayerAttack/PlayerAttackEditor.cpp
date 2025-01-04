// PlayerAttackEditor.cpp
#include "PlayerAttackEditor.h"
#include <externals/imgui/imgui.h>
#include "IPlayerAttack.h"
#include "HorizonMowingDown.h"
#include "WeakDiagonalSlash.h"
#include "lib/myFunc/MathFunc.h"
#include "Engine/core/EngineUI.h"
#include "Game/3d/Player/Player.h"

#include "lib/myFunc/PrimitiveDrawer.h"

#include <cstring> // strncpy を使用するため
#include <externals/nlohmann/json.hpp>
#include <fstream>

// JSONシリアライズ用のto_jsonとfrom_jsonを定義
namespace nlohmann{
	template <>
	struct adl_serializer<Vector3>{
		static void to_json(json& j, const Vector3& v){
			j = json {{"x", v.x}, {"y", v.y}, {"z", v.z}};
		}

		static void from_json(const json& j, Vector3& v){
			j.at("x").get_to(v.x);
			j.at("y").get_to(v.y);
			j.at("z").get_to(v.z);
		}
	};

	template <>
	struct adl_serializer<Quaternion>{
		static void to_json(json& j, const Quaternion& q){
			j = json {{"w", q.w}, {"x", q.x}, {"y", q.y}, {"z", q.z}};
		}

		static void from_json(const json& j, Quaternion& q){
			j.at("w").get_to(q.w);
			j.at("x").get_to(q.x);
			j.at("y").get_to(q.y);
			j.at("z").get_to(q.z);
		}
	};
}


PlayerAttackEditor::PlayerAttackEditor(PlayerAttackController* controller)
	: attackController_(controller), selectedAttackName_(""){
	// 初期バッファ設定
	strncpy_s(newAttackNameBuffer_, "NewAttack", sizeof(newAttackNameBuffer_));
	newAttackNameBuffer_[sizeof(newAttackNameBuffer_) - 1] = '\0'; // ヌル終端

	LoadAttackTemplates("attack_templates.json");

}

void PlayerAttackEditor::ShowGui(){
	ImGui::Begin("Player Attack Editor");

	// 新規攻撃名の入力
	ImGui::InputText("New Attack Name", newAttackNameBuffer_, sizeof(newAttackNameBuffer_));

	// 攻撃テンプレートの追加ボタン
	if (ImGui::Button("Add WeakDiagonalSlash")){
		std::string attackName(newAttackNameBuffer_);
		if (!attackName.empty()){
			attackController_->AddAttackTemplate(attackName, std::make_unique<WeakDiagonalSlash>(attackName));
			// バッファのリセット
			strncpy_s(newAttackNameBuffer_, "NewAttack", sizeof(newAttackNameBuffer_));
			newAttackNameBuffer_[sizeof(newAttackNameBuffer_) - 1] = '\0';
		}
	}

	if (ImGui::Button("Add HorizonMowingDown")){
		std::string attackName(newAttackNameBuffer_);
		if (!attackName.empty()){
			attackController_->AddAttackTemplate(attackName, std::make_unique<HorizonMowingDown>(attackName));
			// バッファのリセット
			strncpy_s(newAttackNameBuffer_, "NewAttack", sizeof(newAttackNameBuffer_));
			newAttackNameBuffer_[sizeof(newAttackNameBuffer_) - 1] = '\0';
		}
	}
	ImGui::SameLine();

	// 保存ボタン
	if (ImGui::Button("Save Templates")){
		SaveAttackTemplates("attack_templates.json");
	}

	ImGui::SameLine();

	ImGui::Separator();
	ImGui::Text("Existing Attack Templates:");

	// 攻撃テンプレートのリスト表示と選択
	auto& templates = const_cast< std::unordered_map<std::string, std::unique_ptr<IPlayerAttack>>& >(attackController_->GetAttackTemplates());
	for (const auto& [attackName, attackTemplate] : templates){
		if (ImGui::Selectable(attackName.c_str(), selectedAttackName_ == attackName)){
			selectedAttackName_ = attackName;
		}
	}

	ImGui::Separator();

	// 選択された攻撃タイプの編集
	auto it = templates.find(selectedAttackName_);
	if (it != templates.end()){
		if (ImGui::CollapsingHeader(("Edit " + selectedAttackName_).c_str())){
			EditControlPoints(it->second);
		}
	}

	ImGui::End();
}

void PlayerAttackEditor::Draw(){
	if (!attackController_->IsAttacking()){
		// 選択された攻撃テンプレートを取得
		auto& templates = attackController_->GetAttackTemplates();
		auto it = templates.find(selectedAttackName_);
		if (it != templates.end()){
			const std::unique_ptr<IPlayerAttack>& attack = it->second;

			// 制御点の取得
			const std::vector<Vector3>& controlPoints = attack->GetControlPoints();

			//武器のワールド座標
			Matrix4x4 worldMatrix = pPlayer_->GetWorldMatrix();

			if (controlPoints.size() >= 4){
				std::vector<Vector3> pointsDrawing;
				const size_t segmentCount = 100;

				// Catmull-Romスプラインの分割点を計算
				for (size_t i = 0; i <= segmentCount; i++){
					float t = 1.0f / segmentCount * i;
					Vector3 pos = CatmullRomPosition(controlPoints, t);
					pos = Vector3::Transform(pos, worldMatrix);
					pointsDrawing.push_back(pos);
				}
				// ラインを描画
				for (size_t i = 0; i < segmentCount; i++){
					PrimitiveDrawer::GetInstance()->DrawLine3d(pointsDrawing[i], pointsDrawing[i + 1], {1.0f, 1.0f, 1.0f, 1.0f});
				}
			}
		}
	}
}


void PlayerAttackEditor::EditControlPoints(std::unique_ptr<IPlayerAttack>& attack){
	// 制御点の取得
	// 具体的な攻撃クラスでのみ制御点を取得できるように動的キャスト
	// ここでは WeakDiagonalSlash と HorizonMowingDown が制御点を持つと仮定
	HorizonMowingDown* swingAttack = dynamic_cast< HorizonMowingDown* >(attack.get());
	WeakDiagonalSlash* slashAttack = dynamic_cast< WeakDiagonalSlash* >(attack.get());

	std::vector<Vector3>* controlPoints = nullptr;

	if (swingAttack){
		controlPoints = &swingAttack->GetControlPoints();
	} else if (slashAttack){
		controlPoints = &slashAttack->GetControlPoints();
	}

	if (controlPoints){
		// 制御点の編集
		for (size_t i = 0; i < controlPoints->size(); ++i){
			std::string label = "Control Point " + std::to_string(i);
			ImGui::DragFloat3(label.c_str(), &(*controlPoints)[i].x, 0.1f);
		}

		// 制御点の追加・削除
		if (ImGui::Button("Add Control Point")){
			controlPoints->emplace_back(Vector3(0.0f, 0.0f, 0.0f));
		}

		if (ImGui::Button("Remove Last Control Point") && !controlPoints->empty()){
			controlPoints->pop_back();
		}

		
	}
}



// 保存機能の実装
void PlayerAttackEditor::SaveAttackTemplates(const std::string& filepath){
	nlohmann::json j;
	auto& templates = attackController_->GetAttackTemplates();
	for (const auto& [attackName, attackTemplate] : templates){
		nlohmann::json attackJson;
		attackJson["type"] = attackTemplate->GetType(); // 攻撃タイプを取得するメソッドを実装する必要があります
		attackJson["controlPoints"] = attackTemplate->GetControlPoints();
		// 必要に応じて他のデータも保存
		j["attacks"][attackName] = attackJson;
	}

	std::ofstream file(filepath);
	if (file.is_open()){
		file << j.dump(4);
		file.close();
	}
}

// 読み込み機能の実装
void PlayerAttackEditor::LoadAttackTemplates(const std::string& filepath){
	nlohmann::json j;
	std::ifstream file(filepath);
	if (file.is_open()){
		file >> j;
		file.close();

		if (j.contains("attacks")){
			auto& attacksJson = j["attacks"];
			for (auto& [attackName, attackJson] : attacksJson.items()){
				std::string type = attackJson["type"];
				std::vector<Vector3> controlPoints = attackJson["controlPoints"].get<std::vector<Vector3>>();

				// 既存の攻撃テンプレートが存在する場合は上書き
				if (attackController_->HasAttackTemplate(attackName)){
					auto& existingAttack = attackController_->GetAttackTemplate(attackName);
					existingAttack.SetControlPoints(controlPoints);
				} else{
					// 新規に攻撃テンプレートを作成
					std::unique_ptr<IPlayerAttack> newAttack;
					if (type == "WeakDiagonalSlash"){
						newAttack = std::make_unique<WeakDiagonalSlash>(attackName);
					} else if (type == "HorizonMowingDown"){
						newAttack = std::make_unique<HorizonMowingDown>(attackName);
					}
					// 他の攻撃タイプを追加する場合はここに分岐を追加

					if (newAttack){
						newAttack->SetControlPoints(controlPoints);
						attackController_->AddAttackTemplate(attackName, std::move(newAttack));
						splineCaches_[attackName] = SplineCache();
					}
				}
			}
		}
	}
}