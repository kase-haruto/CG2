#pragma once
#include"Matrix4x4.h"
#include"Vector3.h"
#include<d3d12.h>

// 平行移動行列
Matrix4x4 MakeTranslateMatrix(const Vector3& translate);
// 拡大縮小行列
Matrix4x4 MakeScaleMatrix(const Vector3& scale);
// 回転行列
Matrix4x4 MakeRotateXMatrix(float theta);
Matrix4x4 MakeRotateYMatrix(float theta);
Matrix4x4 MakeRotateZMatrix(float theta);
Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate);


//正射影行列
Matrix4x4 MakeOrthographicMatrix(float l, float t, float r, float b, float nearClip, float farClip);

ID3D12Resource* CreateBufferResource(ID3D12Device* device, size_t sizeInBytes);