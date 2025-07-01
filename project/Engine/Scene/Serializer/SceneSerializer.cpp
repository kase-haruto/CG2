#include "SceneSerializer.h"

/* ========================================================================
/*  include space
/* ===================================================================== */
#include <Engine/Scene/Context/SceneContext.h>
#include <Engine/Objects/3D/Actor/BaseGameObject.h>
#include <Engine/objects/LightObject/PointLight.h>
#include <Engine/Foundation/Json/JsonUtils.h>
#include <Engine/Objects/3D/Actor/Library/SceneObjectLibrary.h>
#include <Engine/Application/Effects/FxSystem.h>
#include <Engine/Application/Effects/Particle/Object/ParticleSystemObject.h>
#include <Engine/Scene/Utirity/SceneUtility.h>
#include <memory>

bool SceneSerializer::Save(const SceneContext& context, const std::string& path){
	const auto& objects = context.GetObjectLibrary()->GetAllObjects();
	nlohmann::json jArray = nlohmann::json::array();

	for (const auto& obj : objects){
		if (!obj || !obj->IsSerializable()) continue;

		if (auto* config = dynamic_cast< const IConfigurable* >(obj)){
			nlohmann::json j;
			config->ExtractConfigToJson(j);
			j["objectType"] = static_cast< int >(obj->GetObjectType());
			jArray.push_back(j);
		}
	}

	return JsonUtils::Save(path, jArray);
}

bool SceneSerializer::Load(SceneContext& context, const std::string& path){
	nlohmann::json jArray;
	if (!JsonUtils::Load(path, jArray)) return false;

	auto* lib = context.GetObjectLibrary();
	context.Clear();
	lib->Clear();

	for (const auto& j : jArray){
		int type = j.value("objectType", -1);
		std::string name = j.value("name", "UnnamedObject");

		switch (static_cast< ObjectType >(type)){
			case ObjectType::GameObject:
			{
				std::string modelName = j.value("modelName", "debugCube.obj");
				auto ptr = std::make_unique<BaseGameObject>(modelName, name);
				ptr->ConfigurableObject<BaseGameObjectConfig>::ApplyConfigFromJson(j);
				context.AddEditorObject(std::move(ptr)); // 所有権はここに移動
				break;
			}
			case ObjectType::Light:
			{
				if (j.contains("direction")){
					DirectionalLight* dirLight = CreateAndAddObject<DirectionalLight>(&context, "DirectionalLightName");
					dirLight->ApplyConfigFromJson(j);
					context.GetLightLibrary()->SetDirectionalLight(dirLight);
				} else{
					PointLight* pointLight = CreateAndAddObject<PointLight>(&context, "PointLightName");
					pointLight->ApplyConfigFromJson(j);
					context.GetLightLibrary()->SetPointLight(pointLight);
				}
				break;
			}
			case ObjectType::ParticleSystem:
			{
				auto* rawPtr = CreateAndAddObject<ParticleSystemObject>(&context, "ParticleSystem");
				rawPtr->ApplyConfigFromJson(j);
				context.GetFxSystem()->AddEmitter(rawPtr);
				break;
			}

			default:
				continue;
		}
	}

	return true;
}
