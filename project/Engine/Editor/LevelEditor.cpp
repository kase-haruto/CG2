#include "LevelEditor.h"
// engine
#include <Engine/Application/UI/EngineUI/Context/EditorContext.h>

#include <Engine/Objects/3D/Actor/Library/SceneObjectLibrary.h>


void LevelEditor::Initialize(){
	hierarchy_ = std::make_unique<HierarchyPanel>();
	editor_ = std::make_unique<EditorPanel>();
	inspector_ = std::make_unique<InspectorPanel>();
	sceneEditor_ = std::make_unique<SceneObjectEditor>();

	// Panel に LevelEditor 自体を渡す（コールバック通知や setter）
	editor_->SetOnEditorSelected([this] (BaseEditor* editor){
		SetSelectedEditor(editor);
								 });
	hierarchy_->SetOnObjectSelected([this] (SceneObject* obj){
		SetSelectedObject(obj);
									 });
	inspector_->SetSceneObjectEditor(sceneEditor_.get());
}

void LevelEditor::Render(){
	hierarchy_->Render();
	editor_->Render();

	inspector_->SetSelectedEditor(selectedEditor_);
	inspector_->SetSelectedObject(selectedObject_);
	inspector_->Render();
}

void LevelEditor::SetSelectedEditor(BaseEditor* editor){
	selectedEditor_ = editor;
	selectedObject_ = nullptr;
	inspector_->SetSelectedEditor(editor);
}

void LevelEditor::SetSelectedObject(SceneObject* object){
	selectedObject_ = object;
	selectedEditor_ = nullptr;
	hierarchy_->SetSelectedObject(object);
	inspector_->SetSelectedObject(object);
}