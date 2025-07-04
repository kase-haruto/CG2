#include "PrefabSerializer.h"
#include <Engine/Objects/ConfigurableObject/IConfigurable.h>
#include <Engine/Foundation/Json/JsonUtils.h>
#include <Engine/Objects/3D/Actor/SceneObject.h>

#include <Engine/Objects/3D/Actor/BaseGameObject.h>
#include <Engine/Objects/LightObject/PointLight.h>
#include <Engine/Application/Effects/Particle/Object/ParticleSystemObject.h>

#include <unordered_map>
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



std::vector<std::unique_ptr<SceneObject>> PrefabSerializer::Load(const std::string& path){
	nlohmann::json jArray;
	if (!JsonUtils::Load(path, jArray)) return {};

	std::vector<std::unique_ptr<SceneObject>> createdObjects;
	std::unordered_map<Guid, SceneObject*> guidToObjectMap;

	// --- 1st pass: 各オブジェクト生成 ---
	for (const auto& j : jArray){
		int type = j.value("objectType", -1);
		std::string name = j.value("name", "PrefabObject");
		Guid guid = j.value("guid", Guid {});

		std::unique_ptr<SceneObject> obj;

		switch (static_cast< ObjectType >(type)){
			case ObjectType::GameObject:
			{
				std::string modelName = j.value("modelName", "debugCube.obj");
				auto go = std::make_unique<BaseGameObject>(modelName, name);
				go->ApplyConfigFromJson(j);
				obj = std::move(go);
				break;
			}
			case ObjectType::Light:
			{
				auto light = std::make_unique<PointLight>(name);
				light->ApplyConfigFromJson(j);
				obj = std::move(light);
				break;
			}
			case ObjectType::ParticleSystem:
			{
				auto fx = std::make_unique<ParticleSystemObject>(name);
				fx->ApplyConfigFromJson(j);
				obj = std::move(fx);
				break;
			}
			default:
				continue;
		}

		if (obj){
			obj->SetGuid(guid);  // 同じ GUID を維持（または新しくしてもよい）
			guidToObjectMap[guid] = obj.get();
			createdObjects.push_back(std::move(obj));
		}
	}

	// --- 2nd pass: 親子関係の復元 ---
	for (const auto& j : jArray){
		Guid childGuid = j.value("guid", Guid {});
		Guid parentGuid = j.value("parentGuid", Guid {});

		if (childGuid.isValid() && parentGuid.isValid()){
			SceneObject* child = guidToObjectMap[childGuid];
			SceneObject* parent = guidToObjectMap[parentGuid];
			if (child && parent){
				child->SetParent(parent);
			}
		}
	}

	return createdObjects;
}

