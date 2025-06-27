#pragma once

#include <Engine/Objects/3D/Actor/SceneObject.h>
#include <vector>
#include <memory>
#include <algorithm>

class SceneObjectLibrary{
public:
	SceneObjectLibrary();

	// オブジェクトを追加（所有権を受け取る）
	void AddObject(std::unique_ptr<SceneObject> object);

	// 生ポインタで削除要求（内部でunique_ptrから一致を探して消す）
	void RemoveObject(SceneObject* object);

	void Clear();

	template<typename TObject, typename... Args>
	TObject* CreateAndAddObject(Args&&... args);

	// 所有権は返さず、生ポインタのリストを提供
	std::vector<SceneObject*> GetAllObjects() const;

private:
	std::vector<std::unique_ptr<SceneObject>> allSceneObjects_;
};

template<typename TObject, typename ...Args>
inline TObject* SceneObjectLibrary::CreateAndAddObject(Args && ...args){
	auto obj = std::make_unique<TObject>(std::forward<Args>(args)...);
	TObject* raw = obj.get();
	AddObject(std::move(obj));
	return raw;
}