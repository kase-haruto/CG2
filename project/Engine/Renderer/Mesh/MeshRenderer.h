#pragma once
/* ========================================================================
/* include space
/* ===================================================================== */
// engine
#include <Engine/Renderer/Mesh/IMeshRenderable.h>
#include <Engine/Lighting/LightLibrary.h>

// c++
#include <vector>
#include <d3d12.h>

class WorldTransform;
class LightLibrary;

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
	MeshRenderer();

	void Register(IMeshRenderable* renderable, const WorldTransform* transform);
	void Unregister(IMeshRenderable* renderable);
	void DrawAll();
	void Clear();

	void SetLightLibrary(LightLibrary* light){ pLightLibrary_ = light; }

private:
	//===================================================================*/
	//			private methods
	//===================================================================*/
	std::vector<DrawEntry> renderables_;
	LightLibrary* pLightLibrary_;
};
