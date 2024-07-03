#pragma once

#include"Matrix4x4.h"
#include"Vector2.h"
#include"Vector3.h"
#include"Vector4.h"
#include"ModelData.h"
#include"VertexData.h"
#include"Material.h"


#include<chrono>
#include<cstdlib>
#include<d3d12.h>
#include<dxgi1_6.h>
#include<dxcapi.h>
#include<stdint.h>
#include<string>

#pragma comment(lib,"dxcompiler.lib")

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

D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(ID3D12DescriptorHeap* descriptorHeap, uint32_t descriptorSize, uint32_t index);

D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(ID3D12DescriptorHeap* descriptorHeap, uint32_t descriptorSize, uint32_t index);

ModelData LoadObjFile(const std::string& directoryPath, const std::string& filename);

MaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename);

IDxcBlob* CompileShader(
	//CompilerするShaderファイルへのパス
	const std::wstring& filePath,
	//Compilerに使用するProfile
	const wchar_t* profile,
	//初期化で生成したものを3つ
	IDxcUtils* dxcUtils,
	IDxcCompiler3* dxcompiler,
	IDxcIncludeHandler* includeHandler);

