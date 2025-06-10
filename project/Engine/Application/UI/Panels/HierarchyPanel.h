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

class HierarchyPanel : public IEngineUI{
	using OnObjectSelectedCallback = std::function<void(std::shared_ptr<SceneObject>)>;
public:
	HierarchyPanel();
	~HierarchyPanel() override = default;

	void Render() override;
	const std::string& GetPanelName() const override;

	void SetSceneObjectLibrary(const SceneObjectLibrary* library);
	void SetOnObjectSelected(OnObjectSelectedCallback cb){ onObjectSelected_ = std::move(cb); }
	void SetSelectedObject(const std::shared_ptr<SceneObject>& obj){ selected_ = obj; }
	const SceneObjectLibrary* GetSceneObjectLibrary() const { return pSceneObjectLibrary_; }

private:
	const SceneObjectLibrary* pSceneObjectLibrary_ = nullptr;
	OnObjectSelectedCallback onObjectSelected_;
	std::shared_ptr<SceneObject> selected_;
};