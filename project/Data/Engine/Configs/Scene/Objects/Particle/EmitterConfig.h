#pragma once

#include <Data/Engine/Configs/Scene/Objects/Particle/FxParmConfig.h>
#include <Data/Engine/Configs/Scene/Objects/Particle/Module/ModuleConfig.h>
#include <Data/Engine/Configs/Scene/Objects/Particle/Module/ModuleConfigFactory.h>
#include <Engine/Foundation/Math/Vector3.h>
#include <Engine/Foundation/Math/Vector4.h>

#include <nlohmann/json.hpp>
#include <string>
#include <vector>

struct EmitterConfig {
	Vector3 position{};
	Vector4 color;
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
	position = j.value("position", Vector3{ 0, 0, 0 });
	color = j.value("color", Vector4{ 1, 1, 1, 1 });
	velocity = j.value("velocity", FxVector3ParamConfig{});
	lifetime = j.value("lifetime", FxFloatParamConfig{});
	emitRate = j.value("emitRate", 1.0f);
	modelPath = j.value("modelPath", "plane.obj");
	texturePath = j.value("texturePath", "particle.png");
	isDrawEnable = j.value("isDrawEnable", true);
	isComplement = j.value("isComplement", true);
	isStatic = j.value("isStatic", false);

	modules.clear();
	if (j.contains("modules") && j["modules"].is_array()) {
		for (const auto& moduleJson : j["modules"]) {
			auto modConfig = ModuleConfigFactory::FromJson(moduleJson);
			if (modConfig) {
				modules.push_back(std::move(modConfig));
			}
		}
	}
}

inline nlohmann::json EmitterConfig::ToJson() const {
	nlohmann::json j;
	j["position"] = position;
	j["color"] = color;
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