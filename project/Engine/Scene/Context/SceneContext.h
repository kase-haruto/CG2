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
	TObject* AddEditorObject(std::unique_ptr<TObject> object);

	void RemoveEditorObject(SceneObject* obj);
	std::vector<std::unique_ptr<SceneObject>>& GetEditorObjects();

	void AddOnObjectRemovedListener(std::function<void(SceneObject*)> cb){
		objectRemovedCallbacks_.push_back(std::move(cb));
	}

	std::string GetSceneName() const { return sceneName_; }
	void SetSceneName(const std::string& name) {sceneName_ = name;}
private:
	std::unique_ptr<MeshRenderer> renderer_;
	std::unique_ptr<SceneObjectLibrary> objectLibrary_;
	std::unique_ptr<LightLibrary> lightLibrary_;

	std::vector<std::unique_ptr<SceneObject>> editorObjects_;
	std::vector<std::function<void(SceneObject*)>> objectRemovedCallbacks_;

	std::string sceneName_ = "scene"; // シーン名
};

template<typename TObject>
TObject* SceneContext::AddEditorObject(std::unique_ptr<TObject> object) {
	static_assert(std::is_base_of_v<SceneObject, TObject>, "TObject must derive from SceneObject");
	assert(object && "object must be a SceneObject");

	TObject* rawPtr = object.get(); // 所有権を移す前にポインタを取っておく

	objectLibrary_->AddObject(rawPtr);         // 生ポインタで登録
	editorObjects_.emplace_back(std::move(object)); // 所有権はここで移動

	return rawPtr; // 安全に返せる
}