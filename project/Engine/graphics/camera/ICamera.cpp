#include "ICamera.h"
#include <externals/imgui/imgui.h>
#include "engine/objects/SceneObjectManager.h"
#include "Engine/graphics/GraphicsGroup.h"

#include "lib/myFunc/MyFunc.h"

ICamera::ICamera()
:SceneObject(),
worldMatrix_(MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate)){

}

void ICamera::ShowGui(){

	ImGui::Checkbox("Active", &isActive_);
	ImGui::Spacing();

}




const Matrix4x4& ICamera::GetWorldMat() const{
	return worldMatrix_;
}