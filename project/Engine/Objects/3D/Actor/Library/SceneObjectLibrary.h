#pragma once

#include <Engine/Objects/3D/Actor/SceneObject.h>

#include <vector>
#include <memory>

class MeshRenderer;

class SceneObjectLibrary{
public:
	SceneObjectLibrary();

	void AddObject(std::unique_ptr<SceneObject> object);
	void AddObject(const std::shared_ptr<SceneObject>& object);
	void RemoveObject(const std::shared_ptr<SceneObject>& object);
	void Clear();

	const std::vector<std::shared_ptr<SceneObject>>& GetAllObjects() const;

	void RegisterToRenderer(MeshRenderer* renderer);

private:
	std::vector<std::shared_ptr<SceneObject>> allSceneObjects_;
};