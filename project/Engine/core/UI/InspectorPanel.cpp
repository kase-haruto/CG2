#include "InspectorPanel.h"
#include "Engine/objects/SceneObject.h"
#include "Engine/objects/SceneObjectManager.h"
#include <externals/imgui/imgui.h>
#include "HierarchyPanel.h"

InspectorPanel::InspectorPanel()
    : IEngineUI("Inspector"){}

void InspectorPanel::Render(){
    ImGui::Begin(panelName_.c_str());

    const auto& allObjects = SceneObjectManager::GetInstance()->GetAllObjects();

    if (HierarchyPanel::selectedObjectIndex_ >= 0 && HierarchyPanel::selectedObjectIndex_ < static_cast< int >(allObjects.size())){
        SceneObject* selectedObject = allObjects[HierarchyPanel::selectedObjectIndex_];
        if (selectedObject){
            selectedObject->ShowGui();
        } else{
            ImGui::Text("Selected object is invalid.");
        }
    } else{
        ImGui::Text("No object selected.");
    }
    ImGui::End();
}

const std::string& InspectorPanel::GetPanelName() const{
    return panelName_;
}
