#pragma once
/* ========================================================================
/* include space
/* ===================================================================== */
// engine
#include <Engine/Renderer/Mesh/IMeshRenderable.h>
#include <Engine/Lighting/LightLibrary.h>
#include <Engine/Graphics/Pipeline/Service/PipelineService.h>
#include <Engine/System/Observer/SceneObject/IRenderableDeathListener.h>

// c++
#include <vector>
#include <d3d12.h>

class WorldTransform;
class LightLibrary;
class SkyBox;

/* ========================================================================
/* mesh renderer
/* ===================================================================== */
class MeshRenderer
	: public IRenderableDeathListener {
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
	void OnRenderableDestroyed(IMeshRenderable* renderable) override;
	void DrawAll(ID3D12GraphicsCommandList* cmdList);
	void SetSkyBox(SkyBox* skyBox) { skyBox_ = skyBox; }
	void Clear();

	void SetLightLibrary(LightLibrary* light){ pLightLibrary_ = light; }
	void SetPipelineService(PipelineService* service) {
		pipelineService_ = service;
	}

private:
	void DrawGroup(ID3D12GraphicsCommandList* cmdList,const std::vector<DrawEntry>& entries, PipelineType type);

private:
	//===================================================================*/
	//			private methods
	//===================================================================*/
	std::vector<DrawEntry> renderables_;
	PipelineService* pipelineService_;
	LightLibrary* pLightLibrary_;
	SkyBox* skyBox_ = nullptr; ///< SkyBoxの参照。nullptrなら描画しない
};
