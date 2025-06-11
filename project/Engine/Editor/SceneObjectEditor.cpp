#include "SceneObjectEditor.h"
/* ========================================================================
/*		include space
/* ===================================================================== */
// engine
#include <Engine/Objects/3D/Actor/SceneObject.h>
#include <Engine/graphics/Camera/Manager/CameraManager.h>
#include <Engine/Foundation/Utility/Func/MyFunc.h>

// externals
#include <externals/imgui/imgui.h>
#include "externals/imgui/ImGuizmo.h"

SceneObjectEditor::SceneObjectEditor(const std::string& name) :BaseEditor(name) {
	manipulator_ = std::make_unique<Manipulator>();
	manipulator_->SetCamera(CameraManager::GetInstance()->GetDebugCamera());
}

SceneObjectEditor::SceneObjectEditor() :BaseEditor("sceneObjectEditor") {
	manipulator_ = std::make_unique<Manipulator>();
	manipulator_->SetCamera(CameraManager::GetInstance()->GetDebugCamera());
}


void SceneObjectEditor::SetTarget(SceneObject* object) {
	sceneObject_ = object;
	if (object) {
		manipulator_->SetTarget(&sceneObject_->GetWorldTransform());
	} else {
		manipulator_->SetTarget(nullptr);
	}
}

void SceneObjectEditor::Update() {
	manipulator_->Update();
}

void SceneObjectEditor::ShowImGuiInterface() {
	if (!sceneObject_) return;
	sceneObject_->ShowGui();
	// マニピュレーターの更新
	manipulator_->SetTarget(&sceneObject_->GetWorldTransform());
}


//====================================================================//
//  SceneObjectEditor::ShowGuizmo
//====================================================================//
