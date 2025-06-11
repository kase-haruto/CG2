#include "SceneSerializer.h"

/* ========================================================================
/*  include space
/* ===================================================================== */
#include <Engine/Scene/Context/SceneContext.h>
#include <Engine/Objects/3D/Actor/BaseGameObject.h>
#include <Engine/objects/LightObject/PointLight.h>
#include <Engine/Foundation/Json/JsonUtils.h>
#include <Engine/Objects/3D/Actor/Library/SceneObjectLibrary.h>
#include <memory>

bool SceneSerializer::Save(const SceneContext& context, const std::string& path) {
	const auto& objects = context.GetObjectLibrary()->GetAllObjects();
	nlohmann::json jArray = nlohmann::json::array();

	for (const auto& obj : objects) {
		if (!obj || !obj->IsSerializable()) continue;

		if (auto* config = dynamic_cast<const IConfigurable*>(obj)) {
			nlohmann::json j;
			config->ExtractConfigToJson(j);
			j["objectType"] = static_cast<int>(obj->GetObjectType());
			jArray.push_back(j);
		}
	}

	return JsonUtils::Save(path, jArray);
}

bool SceneSerializer::Load(SceneContext& context, const std::string& path) {
	nlohmann::json jArray;
	if (!JsonUtils::Load(path, jArray)) return false;

	auto* lib = context.GetObjectLibrary();
	lib->Clear();

	for (const auto& j : jArray) {
		int type = j.value("objectType", -1);
		std::unique_ptr<SceneObject> obj;

		switch (static_cast<ObjectType>(type)) {
			case ObjectType::GameObject:
				{
					auto ptr = std::make_unique<BaseGameObject>();
					ptr->ConfigurableObject<BaseGameObjectConfig>::ApplyConfigFromJson(j);
					obj = std::move(ptr);
					break;
				}
			case ObjectType::Light:
				{
					if (j.contains("direction")) {
						auto ptr = std::make_unique<DirectionalLight>();
						ptr->ConfigurableObject<DirectionalLightConfig>::ApplyConfigFromJson(j);
						obj = std::move(ptr);
					} else {
						auto ptr = std::make_unique<PointLight>();
						ptr->ConfigurableObject<PointLightConfig>::ApplyConfigFromJson(j);
						obj = std::move(ptr);
					}
					break;
				}
			default:
				continue;
		}

		lib->AddObject(obj.get());
		context.GetEditorObjects().emplace_back(std::move(obj));
	}

	return true;
}