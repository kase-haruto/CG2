#include "InspectorPanel.h"
#include "SelectionManager.h"
#include "Engine/objects/SceneObject.h"
#include "Engine/Editor/BaseEditor.h"
#include "SelectionManager.h"
#include <externals/imgui/imgui.h>

InspectorPanel::InspectorPanel()
    : IEngineUI("Inspector"){}

void InspectorPanel::Render(){
    ImGui::Begin(panelName_.c_str());

    // 選択中のシーンオブジェクト / エディタを取得
    SceneObject* selectedObject = SelectionManager::GetInstance()->GetSelectedObject();
    BaseEditor* selectedEditor = SelectionManager::GetInstance()->GetSelectedEditor();

    // 例：優先してエディタを表示したい場合
    if (selectedEditor != nullptr){
        ImGui::Text("Selected Editor: %s", selectedEditor->GetEditorName().c_str());
        // エディタ固有の表示を行う
        selectedEditor->ShowImGuiInterface();
        ImGui::Separator();
    } else if (selectedObject != nullptr){
        ImGui::Text("Selected Object: %s", selectedObject->GetName().c_str());
        // オブジェクト固有の表示を行う
        selectedObject->ShowGui();
        ImGui::Separator();
    } else{
        ImGui::Text("Nothing is selected.");
    }

    ImGui::End();
}

const std::string& InspectorPanel::GetPanelName() const{
    return panelName_;
}
