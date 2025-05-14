#include "Camera3d.h"
/* ========================================================================
/* include space
/* ===================================================================== */
#include <Engine/Graphics/Context/GraphicsGroup.h>

// lib
#include <Engine/Foundation/Utility/Func/MyFunc.h>

// c++
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
