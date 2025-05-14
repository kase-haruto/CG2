#pragma once
/* ========================================================================
/* include space
/* ===================================================================== */
// engine
#include <Engine/Renderer/Mesh/IMeshRenderable.h>

// c++
#include <vector>
#include <d3d12.h>

struct Matrix4x4;

/* ========================================================================
/* mesh renderer
/* ===================================================================== */
class MeshRenderer{
public:
	//===================================================================*/
	//			public methods
	//===================================================================*/
public:
	MeshRenderer() = default;

	void Register(IMeshRenderable* renderable);
	void Unregister(IMeshRenderable* renderable);
	void DrawAll();
	void Clear();

private:
	//===================================================================*/
	//			private methods
	//===================================================================*/
	std::vector<IMeshRenderable*> renderables_;
};
