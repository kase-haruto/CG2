#pragma once

/* ========================================================================
/* include space
/* ===================================================================== */

#include <Engine/Renderer/Mesh/VertexData.h>
#include <Engine/Objects/Transform/Transform.h>
#include <Engine/Graphics/Buffer/DxIndexBuffer.h>
#include <Engine/Graphics/Buffer/DxVertexBuffer.h>
#include <Engine/Graphics/Buffer/DxConstantBuffer.h>
#include <Engine/Renderer/Mesh/IMeshRenderable.h>
#include <Engine/Objects/3D/Actor/SceneObject.h>

#include <string>
#include <array> 
#include <functional>

struct Vector3;

class SkyBox :
	public IMeshRenderable {
public:
	SkyBox(std::string fileName, std::function<void(IMeshRenderable*, const WorldTransform*)> registerCB);
	~SkyBox()override = default;
	void Initialize();
	void ShowGui();
	void Update();
	void Draw(const WorldTransform& transform)override;

private:
	std::array<VertexData, 24> vertices_;
	std::array<uint16_t, 36> indices_;
	std::string textureName_;

	DxVertexBuffer<VertexData> vertexBuffer_;
	DxIndexBuffer<uint16_t> indexBuffer_;
	WorldTransform worldTransform_;
};

