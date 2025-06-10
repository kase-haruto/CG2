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
	void RegisterAllToRenderer();

	template<typename TObject>
	std::shared_ptr<SceneObject> AddEditorObject(std::shared_ptr<TObject> object);

	void RemoveEditorObject(const std::shared_ptr<SceneObject>& obj);
	std::vector<std::shared_ptr<SceneObject>>& GetEditorObjects();

	void AddOnObjectRemovedListener(std::function<void(SceneObject*)> cb){
		objectRemovedCallbacks_.push_back(std::move(cb));
	}

private:
	std::unique_ptr<MeshRenderer> renderer_;
	std::unique_ptr<SceneObjectLibrary> objectLibrary_;
	std::unique_ptr<LightLibrary> lightLibrary_;

	std::vector<std::shared_ptr<SceneObject>> editorObjects_;
	std::vector<std::function<void(SceneObject*)>> objectRemovedCallbacks_;
};

template<typename TObject>
inline std::shared_ptr<SceneObject> SceneContext::AddEditorObject(std::shared_ptr<TObject> object){
	static_assert(std::is_base_of_v<SceneObject, TObject>, "TObject must derive from SceneObject");
	assert(object && "object must be a SceneObject");

	editorObjects_.emplace_back(object);
	objectLibrary_->AddObject(object);
	return object;
}