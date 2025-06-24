#pragma once
/* ========================================================================
/*      include space
/* ===================================================================== */
// engine
#include <Engine/Application/UI/EngineUI/IEngineUI.h>

class SceneObject;
class SceneObjectLibrary;

// c++
#include <vector>
#include <string>
#include <functional>
#include <memory>

class HierarchyPanel 
	: public IEngineUI {
	using OnObjectSelectedCallback = std::function<void(SceneObject*)>;
public:
	HierarchyPanel();
	~HierarchyPanel() override = default;

	void Render() override;
	void ShowObjectRecursive(SceneObject* obj, int depth);
	void ShowObjectRecursive(SceneObject* obj);
	const std::string& GetPanelName() const override;

	void SetSceneObjectLibrary(const SceneObjectLibrary* library);
	void SetOnObjectSelected(OnObjectSelectedCallback cb) { onObjectSelected_ = std::move(cb); }
	void SetSelectedObject(SceneObject* obj) { selected_ = obj; }
	const SceneObjectLibrary* GetSceneObjectLibrary() const { return pSceneObjectLibrary_; }
	SceneObject* GetSelectedObject() const { return selected_; }

private:
	const SceneObjectLibrary* pSceneObjectLibrary_ = nullptr;
	OnObjectSelectedCallback onObjectSelected_;
	SceneObject* selected_ = nullptr;
};