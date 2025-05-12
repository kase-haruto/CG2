#pragma once

#include <Engine/graphics/VertexData.h>
#include <Engine/objects/Transform.h>
#include <Engine/core/DirectX/Buffer/DxIndexBuffer.h>
#include <Engine/core/DirectX/Buffer/DxVertexBuffer.h>
#include <Engine/core/DirectX/Buffer/DxConstantBuffer.h>
#include <Engine/objects/Mesh/IMeshRenderable.h>
#include <Engine/objects/SceneObject.h>

#include <string>
#include <array> 
#include <functional>
struct Vector3;

class SkyBox :
	public IMeshRenderable,SceneObject {
public:
	SkyBox(std::string fileName,std::function<void(IMeshRenderable*)>);
	~SkyBox()override = default;
	void Initialize();
	void ShowGui()override;
	void Update(const Vector3& center, const Vector3& size);
	void Draw()override;

private:
	std::array<VertexData, 24> vertices_;
	std::array<uint16_t, 36> indices_;
	std::string textureName_;

	DxVertexBuffer<VertexData> vertexBuffer_;
	DxIndexBuffer<uint16_t> indexBuffer_;
	WorldTransform worldTransform_;
};

