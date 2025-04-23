// Camera3d.cpp
#include "Engine/graphics/camera/Camera3d.h"
#include "Engine/graphics/GraphicsGroup.h"
#include "lib/myFunc/MyFunc.h"
#include <externals/imgui/imgui.h>
#include <cmath>

Camera3d::Camera3d()
    : BaseCamera(){
	BaseCamera::SetName("MainCamera");
	transform_.translate = {0.0f, 2.0f, -10.0f};
}

void Camera3d::Update(){
    BaseCamera::Update();
}

void Camera3d::ShowGui(){
	//名前の表示
	SceneObject::ShowGui();

	// アクティブかどうか
	BaseCamera::ShowGui();
}
