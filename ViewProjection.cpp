#include "ViewProjection.h"
#include<cmath>

void ViewProjection::Initialize(uint32_t width, uint32_t height){
	Matrix4x4 cameraMatrix = Matrix4x4::MakeAffineMatrix(transform.scale,
											   transform.rotate,
											   transform.translate
	);
	Matrix4x4 viewMatrix = Matrix4x4::Inverse(cameraMatrix);
	Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, float(width) / float(height), 0.1f, 100.0f);
	viewProjection_ = Matrix4x4::Multiply(viewMatrix, projectionMatrix);
}



Matrix4x4 ViewProjection::MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip){
	Matrix4x4 result;
	result = {
		1 / aspectRatio * 1 / std::tan(fovY / 2),0,0,0,
		0,1 / std::tan(fovY / 2),0,0,
		0,0,farClip / (farClip - nearClip),1,
		0,0,-nearClip * farClip / (farClip - nearClip),0
	};
	return result;
}