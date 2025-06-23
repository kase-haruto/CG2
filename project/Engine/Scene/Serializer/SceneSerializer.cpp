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
	context.Clear();
	lib->Clear();                           // 全オブジェクトクリア（エディタ外含む）

	for (const auto& j : jArray) {
		int type = j.value("objectType", -1);
		std::string name = j.value("name", "UnnamedObject");

		switch (static_cast<ObjectType>(type)) {
			case ObjectType::GameObject:
				{
					std::string modelName = j.value("modelName", "debugCube.obj");
					auto ptr = std::make_unique<BaseGameObject>(modelName, name);
					ptr->ConfigurableObject<BaseGameObjectConfig>::ApplyConfigFromJson(j);
					context.GetMeshRenderer()->Register(ptr->GetModel(), &ptr->GetWorldTransform());
					context.AddEditorObject(std::move(ptr)); // 所有権はここに移動
					break;
				}
			case ObjectType::Light:
				{
					if (j.contains("direction")) {
						auto ptr = std::make_unique<DirectionalLight>(name);
						ptr->ConfigurableObject<DirectionalLightConfig>::ApplyConfigFromJson(j);
						context.GetObjectLibrary()->AddObject(ptr.get());
						context.GetLightLibrary()->SetDirectionalLight(std::move(ptr));
					} else {
						auto ptr = std::make_unique<PointLight>(name);
						ptr->ConfigurableObject<PointLightConfig>::ApplyConfigFromJson(j);
						context.GetObjectLibrary()->AddObject(ptr.get());
						context.GetLightLibrary()->SetPointLight(std::move(ptr));
					}
					break;
				}
			case ObjectType::ParticleSystem:
				{
					break;
				}

			default:
				continue;
		}
	}

	return true;
}
