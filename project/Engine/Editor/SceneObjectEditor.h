#pragma once
/* ========================================================================
/*  include space
/* ===================================================================== */
#include <Engine/Editor/BaseEditor.h>

class SceneObject;

//　sceneオブジェクト編集
class SceneObjectEditor :
    public BaseEditor {
public:
	//===================================================================*/
	//                   public functions
	//===================================================================*/
	SceneObjectEditor(const std::string& name);
	SceneObjectEditor();
	~SceneObjectEditor() override = default;

	void ShowImGuiInterface() override;
	void SetSceneObject(SceneObject* sceneObject) { sceneObject_ = sceneObject; }
	void ShowGuizmo();

private:

	//===================================================================*/
	//                   public functions
	//===================================================================*/
	SceneObject* sceneObject_ = nullptr;	// 編集対象のSceneObject
};

