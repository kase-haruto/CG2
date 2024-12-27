#include "ICamera.h"
#include <externals/imgui/imgui.h>
#include "engine/objects/SceneObjectManager.h"

ICamera::ICamera()
:SceneObject(){

}

void ICamera::ShowGui(){

	ImGui::Checkbox("Active", &isActive_);
	ImGui::Spacing();

}
