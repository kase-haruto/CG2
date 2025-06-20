/* ========================================================================
/*  include space
/* ===================================================================== */

// engine
#include <Engine/Application/UI/Panels/HierarchyPanel.h>
#include <Engine/Application/UI/Panels/InspectorPanel.h>
#include <Engine/Application/UI/EngineUI/Context/EditorContext.h>
#include <Engine/objects/3D/Actor/Library/SceneObjectLibrary.h>
#include <Engine/Objects/3D/Actor/SceneObject.h>


// lib
#include <externals/imgui/imgui.h>
#include <map>

HierarchyPanel::HierarchyPanel() : IEngineUI("Hierarchy") {}

void HierarchyPanel::Render(){
	ImGui::Begin(panelName_.c_str(), nullptr, ImGuiWindowFlags_NoDecoration);
	ImGui::Text("Scene Hierarchy");

	if (!pSceneObjectLibrary_){
		ImGui::Text("SceneObjectLibrary not set.");
		ImGui::End();
		return;
	}

	const auto& allObjects = pSceneObjectLibrary_->GetAllObjects();

	for (SceneObject* obj : allObjects){
		if (!obj || obj->GetParent()) continue; // 親がいないオブジェクトのみ描画
		ShowObjectRecursive(obj);
	}

	ImGui::End();
}

void HierarchyPanel::ShowObjectRecursive(SceneObject* obj){
	bool isSelected = (selected_ == obj);
	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth |
		(obj->GetChildren().empty() ? ImGuiTreeNodeFlags_Leaf : 0) |
		(isSelected ? ImGuiTreeNodeFlags_Selected : 0);

	bool open = ImGui::TreeNodeEx(obj->GetName().c_str(), flags);
	if (ImGui::IsItemClicked()){
		selected_ = obj;
		if (onObjectSelected_) onObjectSelected_(obj);
	}

	// ドラッグ元
	if (ImGui::BeginDragDropSource()){
		ImGui::SetDragDropPayload("SceneObjectPtr", &obj, sizeof(SceneObject*));
		ImGui::Text("Move: %s", obj->GetName().c_str());
		ImGui::EndDragDropSource();
	}

	// ドロップ先
	if (ImGui::BeginDragDropTarget()){
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("SceneObjectPtr")){
			SceneObject* dropped = *static_cast< SceneObject* const* >(payload->Data);
			if (dropped != obj && dropped->GetParent() != obj){
				dropped->SetParent(obj);
			}
		}
		ImGui::EndDragDropTarget();
	}

	if (open){
		for (SceneObject* child : obj->GetChildren()){
			ShowObjectRecursive(child);
		}
		ImGui::TreePop();
	}
}

const std::string& HierarchyPanel::GetPanelName() const{
	return panelName_;
}

void HierarchyPanel::SetSceneObjectLibrary(const SceneObjectLibrary* library){
	pSceneObjectLibrary_ = library;
}
