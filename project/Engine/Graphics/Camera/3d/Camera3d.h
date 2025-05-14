#pragma once

#include <Engine/Graphics/Camera/Base/BaseCamera.h>


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
};
