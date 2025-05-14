#pragma once
#include <vector>
#include <Engine/Renderer/Mesh/VertexData.h>
#include <Engine/core/DirectX/Buffer/DxVertexBuffer.h>
#include <Engine/core/DirectX/Buffer/DxConstantBuffer.h>
#include <Engine/Objects/Transform/Transform.h>

struct Vector3;
struct Vector4;
struct Matrix4x4;

class BoxDrawer {
public:
	void Initialize();
	void DrawBox(const Vector3& center, const Vector3& size, const Vector4& color);
	void Render();
	void Clear();

private:
	std::vector<VertexPosColor> vertices_;
	DxVertexBuffer<VertexPosColor> vertexBuffer_;
	DxConstantBuffer<TransformationMatrix> transformBuffer_;

private:
	static constexpr size_t kMaxBoxes = 256;
};
