#pragma once
/* ========================================================================
/* include space
/* ===================================================================== */
#include <Engine/objects/Mesh/IMeshRenderable.h>

#include <vector>

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
