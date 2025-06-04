#pragma once
/* ========================================================================
/* include space
/* ===================================================================== */

// engine
#include <Engine/Renderer/Mesh/MeshRenderer.h>
#include <Engine/objects/3D/Actor/Library/SceneObjectLibrary.h>
#include <Engine/Lighting/LightLibrary.h>

// c++
#include <memory>


class SceneContext{
public:
	SceneContext();
	~SceneContext();

	void Initialize();
	void Update();

	MeshRenderer* GetMeshRenderer() const{ return renderer_.get(); }
	SceneObjectLibrary* GetObjectLibrary() const{ return objectLibrary_.get(); }
	void RegisterObject(SceneObject* object);
	void RegisterAllToRenderer();

private:
	std::unique_ptr<MeshRenderer> renderer_;
	std::unique_ptr<SceneObjectLibrary> objectLibrary_;
	std::unique_ptr<LightLibrary> lightLibrary_;
};


