#include "PrefabSerializer.h"
#include <Engine/Objects/ConfigurableObject/IConfigurable.h>
#include <Engine/Foundation/Json/JsonUtils.h>
#include <Engine/Objects/3D/Actor/SceneObject.h>
#include <functional>

bool PrefabSerializer::Save(const std::vector<SceneObject*>& roots, const std::string& path){
	nlohmann::json jArray = nlohmann::json::array();

	std::function<void(SceneObject*)> serializeRecursive = [&] (SceneObject* obj){
		if (!obj || !obj->IsSerializable()) return;

		if (auto* config = dynamic_cast< IConfigurable* >(obj)){
			nlohmann::json j;
			config->ExtractConfigToJson(j);
			j["objectType"] = static_cast< int >(obj->GetObjectType());
			j["guid"] = obj->GetGuid(); // そのまま保存
			if (auto parent = obj->GetParent()){
				j["parentGuid"] = parent->GetGuid();
			}

			jArray.push_back(j);
		}

		for (auto* child : obj->GetChildren()){
			serializeRecursive(child);
		}
		};

	for (auto* root : roots){
		serializeRecursive(root);
	}

	return JsonUtils::Save(path, jArray);
}

// Loadは今は空でOK（後で拡張予定）
std::vector<std::unique_ptr<SceneObject>> PrefabSerializer::Load([[maybe_unused]]const std::string& path){
	return {};
}
