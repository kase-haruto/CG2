#pragma once

//engine
#include <Engine/core/DirectX/Buffer/DxConstantBuffer.h>
#include <Engine/graphics/Pipeline/PipelineType.h>

//math
#include <Engine/Foundation/Math/Matrix4x4.h>
#include <Engine/Foundation/Math/Vector3.h>

// 定数バッファ用データ構造体
struct Camera3DForGPU{
	Matrix4x4 view;
	Matrix4x4 projection;
	Matrix4x4 viewProjection;
	Vector3 worldPosition;
	float padding;
};


class Camera3DBuffer{
public:
	//===================================================================*/
	//                   public methods
	//===================================================================*/
	void Initialize(ID3D12Device* device);
	void Update(const Matrix4x4& view, const Matrix4x4& proj, const Vector3& worldPos);
	void SetCommand(ID3D12GraphicsCommandList* cmdList, PipelineType pipelineType);

private:
	//===================================================================*/
	//                   private methods
	//===================================================================*/
	Camera3DForGPU data_;
	DxConstantBuffer<Camera3DForGPU> buffer_;
};

