#include "SceneSwitcherPanel.h"
#include "Engine/scene/SceneManager.h"

#include <externals/imgui/imgui.h>

SceneSwitcherPanel::SceneSwitcherPanel(SceneManager* sceneManager)
    : IEngineUI("Scene Switcher"), sceneManager_(sceneManager){}

void SceneSwitcherPanel::Render(){
    ImGui::Begin(panelName_.c_str(), nullptr, ImGuiWindowFlags_NoCollapse);

    for (size_t i = 0; i < sceneOptions_.size(); ++i){
        const auto& sceneOption = sceneOptions_[i];

        if (ImGui::Button(sceneOption.first.c_str())){
            // (B) ボタンが押されたら RequestSceneChange を呼んで切り替えをリクエスト
            sceneManager_->RequestSceneChange(sceneOption.second);
        }

        if (i < sceneOptions_.size() - 1){
            ImGui::SameLine();
        }
    }

    ImGui::End();
}