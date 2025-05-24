#pragma once
/* ========================================================================
/* include space
/* ===================================================================== */
// engine
#include <Engine/Renderer/Mesh/IMeshRenderable.h>
#include <Engine/Lighting/LightLibrary.h>
#include <Engine/Graphics/Pipeline/Service/PipelineService.h>

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
	void DrawGroup(const std::vector<DrawEntry>& entries, PipelineType type);
	void Clear();

	void SetLightLibrary(LightLibrary* light){ pLightLibrary_ = light; }

private:
	//===================================================================*/
	//			private methods
	//===================================================================*/
	std::unique_ptr<PipelineService> pipelineService_;
	std::vector<DrawEntry> renderables_;
	LightLibrary* pLightLibrary_;
};
