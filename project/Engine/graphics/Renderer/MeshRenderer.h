#pragma once
/* ========================================================================
/* include space
/* ===================================================================== */
#include <Engine/objects/Mesh/IMeshRenderable.h>

#include <vector>

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

private:
	//===================================================================*/
	//			private methods
	//===================================================================*/
	std::vector<IMeshRenderable*> renderables_;
};
