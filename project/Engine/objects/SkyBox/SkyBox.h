#pragma once

/* ========================================================================
/* include space
/* ===================================================================== */

#include <Engine/Renderer/Mesh/VertexData.h>
#include <Engine/Objects/Transform/Transform.h>
#include <Engine/core/DirectX/Buffer/DxIndexBuffer.h>
#include <Engine/core/DirectX/Buffer/DxVertexBuffer.h>
#include <Engine/core/DirectX/Buffer/DxConstantBuffer.h>
#include <Engine/Renderer/Mesh/IMeshRenderable.h>
#include <Engine/Objects/3D/Actor/SceneObject.h>

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
	void Update();
	void Draw()override;

private:
	std::array<VertexData, 24> vertices_;
	std::array<uint16_t, 36> indices_;
	std::string textureName_;

	DxVertexBuffer<VertexData> vertexBuffer_;
	DxIndexBuffer<uint16_t> indexBuffer_;
	WorldTransform worldTransform_;
};

