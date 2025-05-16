/* ========================================================================
/*  include space
/* ===================================================================== */

// engine
#include <Engine/Application/UI/Panels/HierarchyPanel.h>
#include <Engine/Application/UI/Panels/InspectorPanel.h>
#include <Engine/Application/UI/EngineUI/Context/EditorContext.h>
#include <Engine/objects/3D/Actor/Library/SceneObjectLibrary.h>

// lib
#include <externals/imgui/imgui.h>
#include <map>

int HierarchyPanel::selectedObjectIndex_ = -1;

HierarchyPanel::HierarchyPanel()
	:IEngineUI("Hierarchy"){}

void HierarchyPanel::Render(){
	ImGui::Begin(panelName_.c_str(), nullptr, ImGuiWindowFlags_NoDecoration);
	ImGui::Text("Scene Hierarchy");

	const auto& allObjects = pSceneObjectLibrary_->GetAllObjects();

	// カテゴリ分け
	std::map<std::string, std::vector<SceneObject*>> categorizedObjects = {
		{ "Cameras", {} },
		{ "Lights", {} },
		{ "Game Objects", {} }
	};

	for (SceneObject* obj : allObjects){
		if (!obj) continue;
		switch (obj->GetObjectType()){
			case ObjectType::Camera: categorizedObjects["Cameras"].push_back(obj); break;
			case ObjectType::Light: categorizedObjects["Lights"].push_back(obj); break;
			case ObjectType::GameObject: categorizedObjects["Game Objects"].push_back(obj); break;
			default: break;
		}
	}

	// 現在の選択
	SceneObject* selected = pEditorContext_->GetSelectedObject();

	// カテゴリごとの描画
	for (const auto& [category, objects] : categorizedObjects){
		if (ImGui::CollapsingHeader(category.c_str(), ImGuiTreeNodeFlags_DefaultOpen)){
			for (SceneObject* obj : objects){
				bool isSelected = (selected == obj);
				if (ImGui::Selectable(obj->GetName().c_str(), isSelected)){
					pEditorContext_->SetSelectedObject(obj);
					pEditorContext_->SetSelectedEditor(nullptr); // Editorの選択解除
				}
				if (isSelected){
					ImGui::SetItemDefaultFocus();
				}
			}
		}
	}

	ImGui::End();
}

const std::string& HierarchyPanel::GetPanelName() const{
	return panelName_;
}

void HierarchyPanel::SetEditorContext(EditorContext* context){
	pEditorContext_ = context;
}

void HierarchyPanel::SetSceneObjectLibrary(const SceneObjectLibrary* library){
	pSceneObjectLibrary_ = library;
}
