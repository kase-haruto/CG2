#pragma once
/* ========================================================================
/*  include space
/* ===================================================================== */
#include <Engine/Editor/BaseEditor.h>
#include <Engine/Application/UI/EngineUI/Manipulator.h>

#include <memory>

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

	void Update();
	void ShowImGuiInterface() override;

	void SetTarget(const std::shared_ptr<SceneObject>& object);
	Manipulator* GetManipulator() const{ return manipulator_.get(); }
private:
	//===================================================================*/
	//                   private variables
	//===================================================================*/
	std::unique_ptr<Manipulator> manipulator_ = nullptr;	// マニピュレーター
	std::weak_ptr<SceneObject> sceneObject_;					// 編集対象のSceneObject
};

