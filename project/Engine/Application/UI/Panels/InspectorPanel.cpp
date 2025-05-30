/* ========================================================================
/*  include Space
/* ===================================================================== */

// engine
#include <Engine/Application/UI/Panels/InspectorPanel.h>
#include <Engine/Objects/3D/Actor/SceneObject.h>
#include <Engine/Editor/BaseEditor.h>

// externals
#include <externals/imgui/imgui.h>

InspectorPanel::InspectorPanel()
	: IEngineUI("Inspector"){
	pSceneObjectEditor_ = std::make_unique<SceneObjectEditor>("SceneObjectEditor");
	
}

void InspectorPanel::Render(){
	ImGui::Begin(panelName_.c_str());

	if (!pEditorContext_){
		ImGui::Text("Editor context not set.");
		ImGui::End();
		return;
	}
	SceneObject* selectedObject = pEditorContext_->GetSelectedObject();
	pSceneObjectEditor_->SetSceneObject(selectedObject);
	BaseEditor* selectedEditor = pEditorContext_->GetSelectedEditor();

	if (selectedEditor){
		ImGui::Text("Selected Editor: %s", selectedEditor->GetEditorName().c_str());
		selectedEditor->ShowImGuiInterface();
		ImGui::Separator();
	} else if (selectedObject){
		ImGui::Text("Selected Object: %s", selectedObject->GetName().c_str());
		pSceneObjectEditor_->ShowImGuiInterface();
		ImGui::Separator();
	} else{
		ImGui::Text("Nothing is selected.");
	}

	ImGui::End();
}

const std::string& InspectorPanel::GetPanelName() const{
	return panelName_;
}
