/* ========================================================================
/*  include space
/* ===================================================================== */

// engine
#include <Engine/Application/UI/Panels/HierarchyPanel.h>
#include <Engine/Application/UI/Panels/InspectorPanel.h>
#include <Engine/Application/UI/EngineUI/Context/EditorContext.h>
#include <Engine/objects/3D/Actor/Library/SceneObjectLibrary.h>
#include <Engine/Objects/3D/Actor/SceneObject.h>
#include <Engine/Assets/Texture/TextureManager.h>

// lib
#include <externals/imgui/imgui.h>
#include <map>

HierarchyPanel::HierarchyPanel() : IEngineUI("Hierarchy"){
	iconEye_.tex = reinterpret_cast< ImTextureID >(TextureManager::GetInstance()->LoadTexture("UI/Tool/Hierarchy/eyeIcon.png").ptr);
	iconEyeOff_.tex = reinterpret_cast< ImTextureID >(TextureManager::GetInstance()->LoadTexture("UI/Tool/Hierarchy/closedEyeIcon.png").ptr);
	iconCamera_.tex = reinterpret_cast< ImTextureID >(TextureManager::GetInstance()->LoadTexture("UI/Tool/Hierarchy/camIcon.png").ptr);
	iconGameObject_.tex = reinterpret_cast< ImTextureID >(TextureManager::GetInstance()->LoadTexture("UI/Tool/Hierarchy/meshIcon.png").ptr);
	iconParticleSystem_.tex = reinterpret_cast< ImTextureID >(TextureManager::GetInstance()->LoadTexture("UI/Tool/Hierarchy/particleIcon.png").ptr);
	iconLight_.tex = reinterpret_cast< ImTextureID >(TextureManager::GetInstance()->LoadTexture("UI/Tool/Hierarchy/lightIcon.png").ptr);
}

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
	ImGui::PushID(obj);

	// 1. Draw有効/無効アイコンを表示
	const bool isDrawEnabled = obj->IsDrawEnable();
	ImTextureID drawIcon = isDrawEnabled ? iconEye_.tex : iconEyeOff_.tex;

	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
	if (ImGui::ImageButton(drawIcon, iconEye_.size)){
		obj->SetDrawEnable(!isDrawEnabled);
	}
	ImGui::PopStyleVar();

	ImGui::SameLine();

	// オブジェクトタイプに応じたアイコンを取得・表示
	ImTextureID typeIcon = nullptr;
	switch (obj->GetObjectType()){
		case ObjectType::Camera:		typeIcon = iconCamera_.tex; break;
		case ObjectType::Light:			typeIcon = iconLight_.tex; break;
		case ObjectType::GameObject:	typeIcon = iconGameObject_.tex; break;
		case ObjectType::ParticleSystem:typeIcon = iconParticleSystem_.tex; break;
		default:						typeIcon = nullptr; break;
	}

	if (typeIcon){
		ImGui::Image(typeIcon, iconEye_.size);
		ImGui::SameLine();
	}

	// ツリーノードラベル（自動インデント付き）
	ImGuiTreeNodeFlags flags =
		ImGuiTreeNodeFlags_OpenOnArrow |
		(obj->GetChildren().empty() ? ImGuiTreeNodeFlags_Leaf : 0) |
		(isSelected ? ImGuiTreeNodeFlags_Selected : 0);

	bool nodeOpen = ImGui::TreeNodeEx(obj->GetName().c_str(), flags);

	if (ImGui::IsItemClicked()){
		selected_ = obj;
		if (onObjectSelected_) onObjectSelected_(obj);
	}

	// ドラッグ開始（送信側）
	if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)){
		SceneObject* draggedObj = obj; // SceneObject* を直接渡す
		ImGui::SetDragDropPayload("SceneObjectPtr", &draggedObj, sizeof(SceneObject*));
		ImGui::Text("%s", obj->GetName().c_str());
		ImGui::EndDragDropSource();
	}

	// ドロップ受信側
	if (ImGui::BeginDragDropTarget()){
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("SceneObjectPtr")){
			IM_ASSERT(payload->DataSize == sizeof(SceneObject*));
			SceneObject* draggedObj = *( SceneObject** ) payload->Data;
			if (draggedObj && draggedObj != obj && !IsDescendantOf(obj, draggedObj)){
				draggedObj->SetParent(obj);
			}
		}
		ImGui::EndDragDropTarget();
	}


	// 子ノードの再帰表示
	if (nodeOpen){
		for (SceneObject* child : obj->GetChildren()){
			ShowObjectRecursive(child);
		}
		ImGui::TreePop();
	}

	ImGui::PopID();
}

bool HierarchyPanel::IsDescendantOf(SceneObject* parent, SceneObject* child){
	if (!child) return false;
	SceneObject* current = child->GetParent();
	while (current){
		if (current == parent) return true;
		current = current->GetParent();
	}
	return false;
}

const std::string& HierarchyPanel::GetPanelName() const{
	return panelName_;
}

void HierarchyPanel::SetSceneObjectLibrary(const SceneObjectLibrary* library){
	pSceneObjectLibrary_ = library;
}
