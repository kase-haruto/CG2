/* ========================================================================
/*  include space
/* ===================================================================== */

// engine
#include <Engine/Application/UI/Panels/HierarchyPanel.h>
#include <Engine/Application/UI/Panels/InspectorPanel.h>
#include <Engine/Application/UI/EngineUI/SelectionManager.h>
#include <Engine/Objects/3D/Actor/SceneObjectManager.h>

// lib
#include <externals/imgui/imgui.h>

int HierarchyPanel::selectedObjectIndex_ = -1;

HierarchyPanel::HierarchyPanel()
    :IEngineUI("Hierarchy"){}

void HierarchyPanel::Render(){
    ImGui::Begin(panelName_.c_str(),nullptr, ImGuiWindowFlags_NoDecoration);
    ImGui::Text("Scene Hierarchy");

    // 全オブジェクトを取得
    const auto& allObjects = SceneObjectManager::GetInstance()->GetAllObjects();

    // オブジェクトをカテゴリごとに分類
    std::vector<SceneObject*> cameraObjects;
    std::vector<SceneObject*> lightObjects;
    std::vector<SceneObject*> gameObjects;

    for (SceneObject* obj : allObjects){
        if (!obj) continue;

        switch (obj->GetObjectType()){
            case ObjectType::Camera:
                cameraObjects.push_back(obj);
                break;
            case ObjectType::Light:
                lightObjects.push_back(obj);
                break;
            case ObjectType::GameObject:
                gameObjects.push_back(obj);
                break;
            default:
                break;
        }
    }

    // カメラセクション
    if (ImGui::CollapsingHeader("Cameras", ImGuiTreeNodeFlags_DefaultOpen)){
        for (size_t i = 0; i < cameraObjects.size(); ++i){
            bool isSelected = (selectedObjectIndex_ == static_cast< int >(i));
            if (ImGui::Selectable(cameraObjects[i]->GetName().c_str(), isSelected)){
                selectedObjectIndex_ = static_cast< int >(i);
                SelectionManager::GetInstance()->SetSelectedObject(cameraObjects[i]);
                // ★Editor の選択をクリア
                SelectionManager::GetInstance()->ClearSelectedEditor();
            }
            if (isSelected){
                ImGui::SetItemDefaultFocus();
            }
        }
    }
    // ライトセクション
    if (ImGui::CollapsingHeader("Lights", ImGuiTreeNodeFlags_DefaultOpen)){
        for (size_t i = 0; i < lightObjects.size(); ++i){
            bool isSelected = (selectedObjectIndex_ == static_cast< int >(cameraObjects.size() + i));
            if (ImGui::Selectable(lightObjects[i]->GetName().c_str(), isSelected)){
                selectedObjectIndex_ = static_cast< int >(cameraObjects.size() + i);
                SelectionManager::GetInstance()->SetSelectedObject(lightObjects[i]);
                // ★Editor の選択をクリア
                SelectionManager::GetInstance()->ClearSelectedEditor();
            }
            if (isSelected){
                ImGui::SetItemDefaultFocus();
            }
        }
    }
    // ゲームオブジェクトセクション
    if (ImGui::CollapsingHeader("Game Objects", ImGuiTreeNodeFlags_DefaultOpen)){
        for (size_t i = 0; i < gameObjects.size(); ++i){
            bool isSelected = (selectedObjectIndex_ == static_cast< int >(cameraObjects.size() + lightObjects.size() + i));
            if (ImGui::Selectable(gameObjects[i]->GetName().c_str(), isSelected)){
                selectedObjectIndex_ = static_cast< int >(cameraObjects.size() + lightObjects.size() + i);
                SelectionManager::GetInstance()->SetSelectedObject(gameObjects[i]);
                // ★Editor の選択をクリア
                SelectionManager::GetInstance()->ClearSelectedEditor();
            }
            if (isSelected){
                ImGui::SetItemDefaultFocus();
            }
        }
    }

    ImGui::End();
}

const std::string& HierarchyPanel::GetPanelName() const{
    return panelName_;
}