// PlayerAttackEditor.cpp
#include "PlayerAttackEditor.h"
#include <externals/imgui/imgui.h>
#include "IPlayerAttack.h"
#include "HorizonMowingDown.h"
#include "WeakDiagonalSlash.h"
#include "lib/myFunc/MathFunc.h"
#include "Engine/core/EngineUI.h"

#include <cstring> // strncpy を使用するため

PlayerAttackEditor::PlayerAttackEditor(PlayerAttackController* controller)
    : attackController_(controller), selectedAttackName_(""){
    // 初期バッファ設定
    strncpy_s(newAttackNameBuffer_, "NewAttack", sizeof(newAttackNameBuffer_));
    newAttackNameBuffer_[sizeof(newAttackNameBuffer_) - 1] = '\0'; // ヌル終端
}

void PlayerAttackEditor::ShowGui([[maybe_unused]]bool isShow){
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

        ImGui::Separator();

        // 軌道の可視化
        ImGui::Text("Trajectory Visualization:");
        ImDrawList* drawList = ImGui::GetWindowDrawList();
        ImVec2 canvasPos = ImGui::GetCursorScreenPos();
        ImVec2 canvasSize = ImVec2(300, 300);
        ImGui::InvisibleButton("canvas", canvasSize);
        drawList->AddRect(canvasPos, ImVec2(canvasPos.x + canvasSize.x, canvasPos.y + canvasSize.y), IM_COL32(255, 255, 255, 255));

        // スプラインの描画
        if (controlPoints->size() >= 4){
            std::vector<Vector3> splinePoints;
            for (float t = 0.0f; t <= 1.0f; t += 0.02f){
                Vector3 pos = CatmullRomPosition(*controlPoints, t);
                splinePoints.emplace_back(pos);
            }

            // 変換関数（スプライン座標をキャンバス座標に変換）
            auto Transform = [&] (const Vector3& v) -> ImVec2{
                float scale = 100.0f;
                float offsetX = canvasPos.x + canvasSize.x / 2;
                float offsetY = canvasPos.y + canvasSize.y / 2;
                return ImVec2(offsetX + v.x * scale, offsetY - v.y * scale); // Y軸反転
                };

            // スプラインラインの描画
            for (size_t i = 1; i < splinePoints.size(); ++i){
                ImVec2 p1 = Transform(splinePoints[i - 1]);
                ImVec2 p2 = Transform(splinePoints[i]);
                drawList->AddLine(p1, p2, IM_COL32(255, 0, 0, 255), 2.0f);
            }

            // 制御点の描画
            for (const auto& point : *controlPoints){
                ImVec2 p = Transform(point);
                drawList->AddCircleFilled(p, 5.0f, IM_COL32(0, 255, 0, 255));
            }
        }
    }
}
