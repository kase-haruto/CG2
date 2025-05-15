#pragma once
/* ========================================================================
/* include space
/* ===================================================================== */
// engine
#include <Engine/Renderer/Mesh/IMeshRenderable.h>

// c++
#include <vector>
#include <d3d12.h>

class WorldTransform;

/* ========================================================================
/* mesh renderer
/* ===================================================================== */
class MeshRenderer{
private:
	//===================================================================*/
	//			struct
	//===================================================================*/
	struct DrawEntry{
		IMeshRenderable* renderable;
		const WorldTransform* transform;
	};
public:
	//===================================================================*/
	//			public methods
	//===================================================================*/
public:
	MeshRenderer() = default;

	void Register(IMeshRenderable* renderable, const WorldTransform* transform);
	void Unregister(IMeshRenderable* renderable);
	void DrawAll();
	void Clear();

private:
	//===================================================================*/
	//			private methods
	//===================================================================*/
	std::vector<DrawEntry> renderables_;
};
