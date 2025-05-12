#pragma once

#include <Engine/graphics/VertexData.h>
#include <Engine/objects/Transform.h>
#include <Engine/core/DirectX/Buffer/DxIndexBuffer.h>
#include <Engine/core/DirectX/Buffer/DxVertexBuffer.h>
#include <Engine/core/DirectX/Buffer/DxConstantBuffer.h>
#include <Engine/objects/Mesh/IMeshRenderable.h>
#include <array> 
#include <functional>
struct Vector3;

class SkyBox :
	public IMeshRenderable {
public:
	SkyBox(std::function<void(IMeshRenderable*)>);
	void Initialize();
	void Update(const Vector3& center, const Vector3& size);
	void Draw()override;

private:
	std::array<VertexData, 8> vertices_;
	std::array<uint16_t, 36> indices_;

	DxVertexBuffer<VertexData> vertexBuffer_;
	DxIndexBuffer<uint16_t> indexBuffer_;
	WorldTransform worldTransform_;
};

