#include "LevelEditor.h"
// engine
#include <Engine/Application/UI/EngineUI/Context/EditorContext.h>
#include <Engine/Graphics/Camera/Manager/CameraManager.h>
#include <Engine/Objects/3D/Actor/Library/SceneObjectLibrary.h>


void LevelEditor::Initialize(){

	// 各パネルの初期化
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

	// ビューポートの初期化
	mainViewport_ = std::make_unique<Viewport>(ViewportType::VIEWPORT_MAIN, "Game Viewport");
	mainViewport_->SetCamera(CameraManager::GetInstance()->GetCamera3d());
	debugViewport_ = std::make_unique<Viewport>(ViewportType::VIEWPORT_DEBUG, "Debug Viewport");
	debugViewport_->SetCamera(CameraManager::GetDebugCamera());

	// Manipulator をツールとして登録
	if (auto* manipulator = sceneEditor_->GetManipulator()){
		debugViewport_->AddTool(manipulator);
	}

}

void LevelEditor::Render(){
	hierarchy_->Render();
	editor_->Render();

	inspector_->SetSelectedEditor(selectedEditor_);
	inspector_->SetSelectedObject(selectedObject_);
	inspector_->Render();

	sceneEditor_->Update();
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

void LevelEditor::RenderViewport(ViewportType type, const ImTextureID& tex){
	//タイプに応じて描画
	if (type == ViewportType::VIEWPORT_MAIN){
		if (mainViewport_){
			mainViewport_->Render(tex);
		}
	} else if (type == ViewportType::VIEWPORT_DEBUG){
		if (debugViewport_){
			debugViewport_->Render(tex);
		}
	}
}
