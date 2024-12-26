// Camera3d.cpp
#include "Engine/graphics/camera/Camera3d.h"
#include "Engine/graphics/GraphicsGroup.h"
#include "lib/myFunc/MyFunc.h"
#include <externals/imgui/imgui.h>
#include <cmath>

Camera3d::Camera3d()
    : BaseCamera(){
	SetName("mainCamera", ObjectType::Camera);

}

void Camera3d::Update(){
    BaseCamera::Update();
}
