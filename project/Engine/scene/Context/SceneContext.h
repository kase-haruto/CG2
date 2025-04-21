#pragma once
/* ========================================================================
/* include space
/* ===================================================================== */

// engine
#include <Engine/graphics/Renderer/MeshRenderer.h>

// c++
#include <memory>

class SceneContext{
public:
	//===================================================================*/
	//			methods
	//===================================================================*/
	SceneContext();
	~SceneContext() = default;
	std::unique_ptr<MeshRenderer> meshRenderer_;
};

