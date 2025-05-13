#include "SelectionManager.h"
#include "Engine/objects/SceneObject.h"
#include "Engine/Editor/BaseEditor.h"

void SelectionManager::SetSelectedObject(SceneObject* obj){
    selectedObject_ = obj;
}

SceneObject* SelectionManager::GetSelectedObject() const{
    return selectedObject_;
}

void SelectionManager::SetSelectedEditor(BaseEditor* editor){
    selectedEditor_ = editor;
}

BaseEditor* SelectionManager::GetSelectedEditor() const{
    return selectedEditor_;
}

void SelectionManager::ClearSelectedObject(){
	selectedObject_ = nullptr;
}

void SelectionManager::ClearSelectedEditor(){
	selectedEditor_ = nullptr;
}
