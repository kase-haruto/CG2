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
#include <Data/Engine/Prefab/Serializer/PrefabSerializer.h>
// lib
#include <externals/imgui/imgui.h>
#include <externals/imgui/ImGuiFileDialog.h>
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
		if (!obj || obj->GetParent()) continue;
		ShowObjectRecursive(obj);
	}

	// 空白クリックで選択解除
	if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(0) && !ImGui::IsAnyItemHovered()){
		selected_ = nullptr;
		if (onObjectSelected_) onObjectSelected_(nullptr);
	}

	// 右クリックされた瞬間にポップアップを開く
	if (ImGui::IsWindowHovered() && !ImGui::IsAnyItemHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Right)){
		ImGui::OpenPopup("BlankContextMenu");
	}

	// ポップアップの表示処理
	if (ImGui::BeginPopup("BlankContextMenu")){
		if (ImGui::MenuItem("Load Prefab")){
			showLoadPrefabDialog_ = true;
		}
		ImGui::EndPopup();
	}


	// ----------- ダイアログ表示の処理（ここから下は End の直前に配置） -----------
		// Prefabロードダイアログオープン
	if (showLoadPrefabDialog_){
		IGFD::FileDialogConfig config;
		config.path = "Resources/Assets/Prefabs/";
		ImGuiFileDialog::Instance()->OpenDialog(
			"LoadPrefabDlg",
			"Load Prefab File",
			".prefab",
			config
		);
		showLoadPrefabDialog_ = false;
	}

	// Prefab保存ダイアログオープン
	if (showSavePrefabDialog_){
		IGFD::FileDialogConfig config;
		config.path = "Resources/Assets/Prefabs/";
		ImGuiFileDialog::Instance()->OpenDialog(
			"SavePrefabDlg",
			"Save Prefab As",
			".prefab",
			config
		);
		showSavePrefabDialog_ = false;
	}

	// 保存ダイアログ処理
	if (ImGuiFileDialog::Instance()->Display("SavePrefabDlg")){
		if (ImGuiFileDialog::Instance()->IsOk()){
			std::string savePath = ImGuiFileDialog::Instance()->GetFilePathName();
			if (prefabSaveTarget_){
				std::vector<SceneObject*> roots = {prefabSaveTarget_};
				PrefabSerializer::Save(roots, savePath);
			}
		}
		ImGuiFileDialog::Instance()->Close();
		prefabSaveTarget_ = nullptr;
	}


	// ロードダイアログ処理
	if (ImGuiFileDialog::Instance()->Display("LoadPrefabDlg")){
		if (ImGuiFileDialog::Instance()->IsOk()){
			std::string loadPath = ImGuiFileDialog::Instance()->GetFilePathName();
			auto loadedObjects = PrefabSerializer::Load(loadPath);
			for (auto& obj : loadedObjects){
				if (pSceneObjectLibrary_ && onObjectCreate_){
					onObjectCreate_(std::move(obj));
				}
			}
		}
		ImGuiFileDialog::Instance()->Close();
	}

	ImGui::End();
}

void HierarchyPanel::ShowObjectRecursive(SceneObject* obj){
	bool isSelected = (selected_ == obj);
	ImGui::PushID(obj);

	const bool isDrawEnabled = obj->IsDrawEnable();
	ImTextureID drawIcon = isDrawEnabled ? iconEye_.tex : iconEyeOff_.tex;

	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
	if (ImGui::ImageButton(drawIcon, iconEye_.size)){
		obj->SetDrawEnable(!isDrawEnabled);
	}
	ImGui::PopStyleVar();

	ImGui::SameLine();

	ImTextureID typeIcon = nullptr;
	switch (obj->GetObjectType()){
		case ObjectType::Camera:         typeIcon = iconCamera_.tex; break;
		case ObjectType::Light:          typeIcon = iconLight_.tex; break;
		case ObjectType::GameObject:     typeIcon = iconGameObject_.tex; break;
		case ObjectType::ParticleSystem: typeIcon = iconParticleSystem_.tex; break;
		default:                         typeIcon = nullptr; break;
	}

	if (typeIcon){
		ImGui::Image(typeIcon, iconEye_.size);
		ImGui::SameLine();
	}

	ImGuiTreeNodeFlags flags =
		ImGuiTreeNodeFlags_OpenOnArrow |
		(obj->GetChildren().empty() ? ImGuiTreeNodeFlags_Leaf : 0) |
		(isSelected ? ImGuiTreeNodeFlags_Selected : 0);

	bool nodeOpen = ImGui::TreeNodeEx(obj->GetName().c_str(), flags);

	if (ImGui::IsItemClicked()){
		selected_ = obj;
		if (onObjectSelected_) onObjectSelected_(obj);
	}

	// オブジェクト上の右クリックメニュー
	if (ImGui::BeginPopupContextItem("SceneObjectContextMenu")){
		if (ImGui::MenuItem("Rename")){
			// TODO: Rename logic
		}
		if (ImGui::MenuItem("Delete")){
			if (onObjectDelete_){
				onObjectDelete_(obj);
			}
		}
		if (ImGui::MenuItem("Create Prefab")){
			prefabSaveTarget_ = obj;
			showSavePrefabDialog_ = true;
		}
		ImGui::EndPopup();
	}

	// ドラッグアンドドロップ
	if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)){
		SceneObject* draggedObj = obj;
		ImGui::SetDragDropPayload("SceneObjectPtr", &draggedObj, sizeof(SceneObject*));
		ImGui::Text("%s", obj->GetName().c_str());
		ImGui::EndDragDropSource();
	}

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
