#pragma once

#include <Data/Engine/Configs/Scene/Objects/Particle/FxParmConfig.h>
#include <Data/Engine/Configs/Scene/Objects/Particle/Module/ModuleConfig.h>
#include <Data/Engine/Configs/Scene/Objects/Particle/Module/ModuleConfigFactory.h>
#include <Engine/Foundation/Math/Vector3.h>

#include <nlohmann/json.hpp>
#include <string>
#include <vector>

struct EmitterConfig {
	Vector3 position{};
	FxVector3ParamConfig velocity;
	FxFloatParamConfig lifetime;

	float emitRate = 0.1f;
	std::string modelPath = "plane.obj";
	std::string texturePath = "particle.png";

	bool isDrawEnable = true;
	bool isComplement = true;
	bool isStatic = false;

	std::vector<std::unique_ptr<BaseModuleConfig>> modules;

	// JSON読み込み用（カスタム実装）
	void FromJson(const nlohmann::json& j);

	// JSON書き込み用（カスタム実装）
	nlohmann::json ToJson() const;
};

inline void EmitterConfig::FromJson(const nlohmann::json& j) {
	position = j.at("position").get<Vector3>();
	velocity = j.at("velocity").get<FxVector3ParamConfig>();
	lifetime = j.at("lifetime").get<FxFloatParamConfig>();
	emitRate = j.at("emitRate").get<float>();
	modelPath = j.at("modelPath").get<std::string>();
	texturePath = j.at("texturePath").get<std::string>();
	isDrawEnable = j.at("isDrawEnable").get<bool>();
	isComplement = j.at("isComplement").get<bool>();
	isStatic = j.at("isStatic").get<bool>();

	modules.clear();
	for (const auto& moduleJson : j.at("modules")) {
		auto modConfig = ModuleConfigFactory::FromJson(moduleJson);
		if (modConfig) {
			modules.push_back(std::move(modConfig));
		}
	}
}

inline nlohmann::json EmitterConfig::ToJson() const {
	nlohmann::json j;
	j["position"] = position;
	j["velocity"] = velocity;
	j["lifetime"] = lifetime;
	j["emitRate"] = emitRate;
	j["modelPath"] = modelPath;
	j["texturePath"] = texturePath;
	j["isDrawEnable"] = isDrawEnable;
	j["isComplement"] = isComplement;
	j["isStatic"] = isStatic;

	j["modules"] = nlohmann::json::array();
	for (const auto& mod : modules) {
		j["modules"].push_back(mod->ToJson());
	}

	return j;
}

inline void to_json(nlohmann::json& j, const EmitterConfig& c) {
	j = c.ToJson();
}

inline void from_json(const nlohmann::json& j, EmitterConfig& c) {
	c.FromJson(j);
}