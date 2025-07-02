#pragma once
/* ========================================================================
/*      include space
/* ===================================================================== */

// engine
#include <Engine/Application/UI/EngineUI/IEngineUI.h>

// c++
#include <vector>
#include <string>
#include <functional>
#include <memory>

// externals
#include <externals/imgui/imgui.h>

// forward
class SceneObject;
class SceneObjectLibrary;

//===================================================================*//
//					HierarchyPanel
//===================================================================*//
class HierarchyPanel 
	: public IEngineUI {
	using OnObjectSelectedCallback = std::function<void(SceneObject*)>;
public:
	//===================================================================*/
	//					public methods
	//===================================================================*/
	HierarchyPanel();
	~HierarchyPanel() override = default;

	void Render() override;
	void ShowObjectRecursive(SceneObject* obj);
	bool IsDescendantOf(SceneObject* parent, SceneObject* child);
	const std::string& GetPanelName() const override;

	//--------- accessor -----------------------------------------------------
	void SetSceneObjectLibrary(const SceneObjectLibrary* library);
	void SetOnObjectSelected(OnObjectSelectedCallback cb) { onObjectSelected_ = std::move(cb); }
	void SetOnObjectDelete(const std::function<void(SceneObject*)>& callback){onObjectDelete_ = callback;}
	void SetSelectedObject(SceneObject* obj) { selected_ = obj; }
	const SceneObjectLibrary* GetSceneObjectLibrary() const { return pSceneObjectLibrary_; }
	SceneObject* GetSelectedObject() const { return selected_; }

	//===================================================================*/
	//					private methods
	//===================================================================*/
private:
	const SceneObjectLibrary* pSceneObjectLibrary_ = nullptr;
	OnObjectSelectedCallback onObjectSelected_;
	SceneObject* selected_ = nullptr;
	std::function<void(SceneObject*)> onObjectDelete_;
private:
	// アイコン
	struct Icon{
		ImTextureID  tex = nullptr;
		ImVec2 size {24.0f,24.0f};
	};

public:
	Icon iconEye_;
	Icon iconEyeOff_;
	Icon iconCamera_;
	Icon iconLight_;
	Icon iconGameObject_;
	Icon iconParticleSystem_;
};