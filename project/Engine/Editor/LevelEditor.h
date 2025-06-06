#pragma once
#include <Engine/Editor/SceneObjectEditor.h>
#include <Engine/Application/UI/Panels/EditorPanel.h>
#include <Engine/Application/UI/Panels/HierarchyPanel.h>
#include <Engine/Application/UI/Panels/InspectorPanel.h>
#include <Engine/Application/UI/EngineUI/Manipulator.h>
#include <Engine/Application/UI/EngineUI/Viewport.h>

//c++
#include <memory>

class BaseEditor;
class SceneObject;

class LevelEditor{
public:
	void Initialize();
	void Render();

	// 編集対象
	void SetSelectedEditor(BaseEditor* editor);
	void SetSelectedObject(SceneObject* object);
	void RenderViewport(ViewportType type, const ImTextureID& tex);

	HierarchyPanel* GetHierarchyPanel() const{ return hierarchy_.get(); }
	EditorPanel* GetEditorPanel() const{ return editor_.get(); }

private:
	// 管理UI
	std::unique_ptr<HierarchyPanel> hierarchy_;
	std::unique_ptr<EditorPanel> editor_;
	std::unique_ptr<InspectorPanel> inspector_;
	std::unique_ptr<SceneObjectEditor> sceneEditor_;

	// ビューポート
	std::unique_ptr<Viewport> mainViewport_;				//< メインビューポート
	std::unique_ptr<Viewport> debugViewport_;				//< デバッグビューポート

	// 状態
	BaseEditor* selectedEditor_ = nullptr;
	SceneObject* selectedObject_ = nullptr;
};