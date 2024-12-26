#include "ICamera.h"
#include <externals/imgui/imgui.h>
#include "engine/objects/SceneObjectManager.h"

ICamera::ICamera()
:SceneObject(){

	//リストに追加
	SceneObjectManager::GetInstance()->AddObject(this);
	objectType_ = ObjectType::Camera;

}

void ICamera::ShowGui(){

	ImGui::Checkbox("Active", &isActive_);
	ImGui::Spacing();

}
