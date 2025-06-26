#pragma once

#include <Engine/Objects/3D/Actor/SceneObject.h>

#include <vector>

class MeshRenderer;

class SceneObjectLibrary {
public:
	SceneObjectLibrary();
	void AddObject(SceneObject* object);
	void RemoveObject(SceneObject* object);
	void Clear();

	const std::vector<SceneObject*>& GetAllObjects() const;


	void RegisterToRenderer(MeshRenderer* renderer);

private:
	std::vector<SceneObject*> allSceneObjects_;
};

