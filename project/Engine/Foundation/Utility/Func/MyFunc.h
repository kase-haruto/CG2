#pragma once

/* math */
#include <Engine/Foundation/Math/Matrix4x4.h>
#include <Engine/Foundation/Math/Vector2.h>
#include <Engine/Foundation/Math/Vector3.h>
#include <Engine/Foundation/Math/Vector4.h>

/* engine */
#include <Engine/Assets/Animation/AnimationStruct.h>
#include <Engine/Assets/Model/ModelData.h>
#include <Engine/Renderer/Mesh/VertexData.h>
#include "engine/graphics/Material.h"
#include "engine/physics/AABB.h"

/* c++ */
#include <list>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <dxgidebug.h>
#include <dxcapi.h>
#include <DirectXTex.h>
#include <stdint.h>
#include <string>
#include <random>
#include <memory>
#include<assimp/scene.h>

class  Model;

// 平行移動行列
Matrix4x4 MakeTranslateMatrix(const Vector3& translate);
// 拡大縮小行列
Matrix4x4 MakeScaleMatrix(const Vector3& scale);
// 回転行列
Matrix4x4 MakeRotateXMatrix(float theta);
Matrix4x4 MakeRotateYMatrix(float theta);
Matrix4x4 MakeRotateZMatrix(float theta);
Matrix4x4 EulerToMatrix(const Vector3& euler);
Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate);
Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Quaternion& rotate, const Vector3& translate);

//正射影行列
Matrix4x4 MakeOrthographicMatrix(float l, float t, float r, float b, float nearClip, float farClip);

Microsoft::WRL::ComPtr<ID3D12Resource>CreateBufferResource(Microsoft::WRL::ComPtr<ID3D12Device> device, size_t sizeInBytes);

D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(ID3D12DescriptorHeap* descriptorHeap, uint32_t descriptorSize, uint32_t index);

D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(ID3D12DescriptorHeap* descriptorHeap, uint32_t descriptorSize, uint32_t index);

ModelData LoadObjFile(const std::string& directoryPath, const std::string& filename);

MaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename);

Animation LoadAnimationFile(const std::string& directoryPath, const std::string& filename);

DirectX::ScratchImage LoadTextureImage(const std::string& filePath);

bool IsCollision(const AABB& aabb, const Vector3& point);

Vector3 TransformNormal(const Vector3& v, const Matrix4x4& m);

float Lerp(float v1, float v2, float t);

float LerpShortAngle(float a, float b, float t);

Vector3 ExtractEulerAnglesFromMatrix(const Matrix4x4& worldMatrix);

Vector2 WorldToScreen(const Vector3& worldPos);

Vector4 MultiplyMatrixVector(const Matrix4x4& mat, const Vector4& vec);

bool WorldToScreen(const Vector3& worldPos, Vector2& outScreenPos);

// 4x4 行列から Scale, Rotate (Euler角), Translation を抽出する関数
void DecomposeMatrix(const Matrix4x4& mat, Vector3& outScale, Vector3& outRotate, Vector3& outTrans);

// skeletonの作成
Skeleton CreateSkeleton(const Node& rootNode);

// jointの作成
int32_t CreateJoint(const Node& node,
					const std::optional<int32_t>& parent,
					std::vector<Joint>& joints);

Node ConvertAssimpNode(const aiNode* assimpNode);

// skinClusterの作成
SkinCluster CreateSkinCluster(const Microsoft::WRL::ComPtr<ID3D12Device>& device,
							  const Skeleton& skeleton,const ModelData& modelData);


//ビルボードの関数
Matrix4x4 MakeYAxisBillboard(const Matrix4x4& cameraMatrix);

Matrix4x4 MakeXAxisBillboard(const Matrix4x4& cameraMatrix);
Matrix4x4 MakeZAxisBillboard(const Matrix4x4& cameraMatrix);