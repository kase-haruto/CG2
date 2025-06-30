#pragma once

#include <memory>
#include <externals/nlohmann/json.hpp>
#include "ModuleConfig.h"

namespace ModuleConfigFactory {
	inline std::unique_ptr<BaseModuleConfig> FromJson(const nlohmann::json& j) {
		if (!j.contains("name")) return nullptr;
		std::string name = j.at("name").get<std::string>();

		std::unique_ptr<BaseModuleConfig> modConfig;

		if (name == "GravityModule") {
			modConfig = std::make_unique<GravityModuleConfig>();
		} else if (name == "SizeOverLifetimeModule") {
			modConfig = std::make_unique<SizeOverLifetimeConfig>();
		} else {
			// ベースクラスで fallback (必要なら)
			modConfig = nullptr;
		}

		if (modConfig) {
			modConfig->FromJson(j);
		}

		return modConfig;
	}
}