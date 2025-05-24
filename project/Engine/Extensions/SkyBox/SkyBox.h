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
#include <optional>

struct Vector3;

class SkyBox :
	public SceneObject, public IMeshRenderable {
public:
	SkyBox(const std::string& fileName,
		   std::optional<std::string> objectName);
	~SkyBox()override = default;
	void Initialize();
	void ShowGui();
	void Update();
	void Draw(const WorldTransform& transform)override;
	const WorldTransform& GetWorldTransform()const;
	void RegisterToRenderer(MeshRenderer* renderer) override;
	BlendMode GetBlendMode() const override { return BlendMode::NONE; }
	//* config ================================================================*/

private:
	std::array<VertexData, 24> vertices_;
	std::array<uint16_t, 36> indices_;
	std::string textureName_;

	DxVertexBuffer<VertexData> vertexBuffer_;
	DxIndexBuffer<uint16_t> indexBuffer_;
	WorldTransform worldTransform_;
};

