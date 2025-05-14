#include "SceneSwitcherPanel.h"
#include <Engine/Scene/System/SceneManager.h>

#include <externals/imgui/imgui.h>

SceneSwitcherPanel::SceneSwitcherPanel(SceneManager* sceneManager)
    : IEngineUI("Scene Switcher"), sceneManager_(sceneManager){}

void SceneSwitcherPanel::Render(){
    ImGui::Begin(panelName_.c_str(), nullptr, ImGuiWindowFlags_NoCollapse);

    // プルダウンで選択するための変数
    static int currentSceneIndex = 1; // 現在選択されているシーンのインデックス

    // プルダウンメニューのラベル
    const char* currentSceneLabel = sceneOptions_[currentSceneIndex].first.c_str();

    // プルダウンメニューの開始
    if (ImGui::BeginCombo("##SceneDropdown", currentSceneLabel)){
        for (size_t i = 0; i < sceneOptions_.size(); ++i){
            const auto& sceneOption = sceneOptions_[i];

            // 各シーンオプションを選択可能にする
            bool isSelected = (currentSceneIndex == i);
            if (ImGui::Selectable(sceneOption.first.c_str(), isSelected)){
                // 選択されたらインデックスを更新し、シーンを切り替える
                currentSceneIndex = static_cast< int >(i);
                sceneManager_->RequestSceneChange(sceneOption.second);
            }

            // 現在選択中の項目にチェックマークを付ける
            if (isSelected){
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }

    ImGui::End();
}
