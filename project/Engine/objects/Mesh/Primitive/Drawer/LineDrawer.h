#pragma once
#include <vector>
#include <Engine/graphics/VertexData.h>
#include <Engine/core/DirectX/Buffer/DxVertexBuffer.h>
#include <Engine/core/DirectX/Buffer/DxConstantBuffer.h>
#include <Engine/objects/Transform.h>

#include<vector>

struct Vector3;
struct Vector4;
struct Matrix4x4;

class LineDrawer{
public:
	void Initialize();
	void DrawLine(const Vector3& start, const Vector3& end, const Vector4& color);
	void Render(const Matrix4x4& vp);
	void Clear();

private:
	std::vector<VertexPosColor> vertices_;
	DxVertexBuffer<VertexPosColor> vertexBuffer_;
	DxConstantBuffer<TransformationMatrix> wvpBuffer_;

private:
	static constexpr size_t kMaxLines = 512; // 最大描画数
};
