// Camera3d.h
#pragma once

#include "Engine/graphics/camera/BaseCamera.h"


class Camera3d
	: public BaseCamera{
public:
	//==================================================================*//
	//			public functions
	//==================================================================*//
	Camera3d();
	~Camera3d() = default;

	void Update() override;  //< 更新
	void ShowGui() override; //< ImGuiによるGUI表示

	void SetCamera(const Vector3& pos, const Vector3& rotate){
		transform_.translate = pos;
		transform_.rotate = rotate;
	}
};
