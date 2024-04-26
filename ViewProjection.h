#pragma once
#include"Transform.h"
#include"Matrix4x4.h"
#include<stdint.h>

class ViewProjection{
private:
	Transform transform = {{1.0f,1.0f,1.0f},
						   {0.0f,0.0f,0.0f},
						   {0.0f,0.0f,-5.0f}
	};

	Matrix4x4 viewProjection_;
private:
	Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip);

public:
	void Initialize(uint32_t width = 1280, uint32_t height = 720);

	Matrix4x4 GetViewProjection()const{ return viewProjection_; }
};