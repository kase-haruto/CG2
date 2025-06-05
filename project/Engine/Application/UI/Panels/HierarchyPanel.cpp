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

HierarchyPanel::HierarchyPanel() : IEngineUI("Hierarchy"){}

void HierarchyPanel::Render(){
	ImGui::Begin(panelName_.c_str(), nullptr, ImGuiWindowFlags_NoDecoration);
	ImGui::Text("Scene Hierarchy");

	if (!pSceneObjectLibrary_){
		ImGui::Text("SceneObjectLibrary not set.");
		ImGui::End();
		return;
	}

	const auto& allObjects = pSceneObjectLibrary_->GetAllObjects();

	std::vector<std::pair<std::string, std::vector<SceneObject*>>> categorizedObjects = {
		{ "Cameras", {} },
		{ "Lights", {} },
		{ "Game Objects", {} }
	};

	for (SceneObject* obj : allObjects){
		if (!obj) continue;

		switch (obj->GetObjectType()){
			case ObjectType::Camera: categorizedObjects[0].second.push_back(obj); break;
			case ObjectType::Light: categorizedObjects[1].second.push_back(obj); break;
			case ObjectType::GameObject: categorizedObjects[2].second.push_back(obj); break;
			default: break;
		}
	}

	for (const auto& [category, objects] : categorizedObjects){
		if (ImGui::CollapsingHeader(category.c_str(), ImGuiTreeNodeFlags_DefaultOpen)){

			std::map<std::string, std::vector<SceneObject*>> groupedObjects;
			for (SceneObject* obj : objects){
				groupedObjects[obj->GetName()].push_back(obj);
			}

			for (const auto& [name, group] : groupedObjects){
				if (group.size() > 1){
					if (ImGui::TreeNode(name.c_str())){
						for (size_t i = 0; i < group.size(); ++i){
							SceneObject* obj = group[i];
							std::string displayName = name + " (" + std::to_string(i) + ")";
							bool isSelected = (selected_ == obj);
							if (ImGui::Selectable(displayName.c_str(), isSelected)){
								selected_ = obj;
								if (onObjectSelected_) onObjectSelected_(obj);
							}
							if (isSelected) ImGui::SetItemDefaultFocus();
						}
						ImGui::TreePop();
					}
				} else{
					SceneObject* obj = group[0];
					bool isSelected = (selected_ == obj);
					if (ImGui::Selectable(name.c_str(), isSelected)){
						selected_ = obj;
						if (onObjectSelected_) onObjectSelected_(obj);
					}
					if (isSelected) ImGui::SetItemDefaultFocus();
				}
			}
		}
	}

	ImGui::End();
}

const std::string& HierarchyPanel::GetPanelName() const{
	return panelName_;
}

void HierarchyPanel::SetSceneObjectLibrary(const SceneObjectLibrary* library){
	pSceneObjectLibrary_ = library;
}