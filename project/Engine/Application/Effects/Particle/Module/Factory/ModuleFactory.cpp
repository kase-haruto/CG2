#include "ModuleFactory.h"

namespace FxModuleFactory {
	std::unique_ptr<BaseFxModule> CreateFromConfig(const BaseModuleConfig& config) {
		if (config.name == "GravityModule") {
			const auto* gConfig = dynamic_cast<const GravityModuleConfig*>(&config);
			if (!gConfig) return nullptr;
			auto mod = std::make_unique<GravityModule>(gConfig->name);
			mod->SetEnabled(gConfig->enabled);
			mod->SetGravity(gConfig->gravity);
			return mod;
		} else if (config.name == "SizeOverLifetimeModule") {
			const auto* sConfig = dynamic_cast<const SizeOverLifetimeConfig*>(&config);
			if (!sConfig) return nullptr;
			auto mod = std::make_unique<SizeOverLiftimeModule>(sConfig->name);
			mod->SetEnabled(sConfig->enabled);
			mod->SetIsGrowing(sConfig->isGrowing);
			mod->SetEaseType(sConfig->easeType);
			return mod;
		}
		return nullptr;
	}

	std::unique_ptr<BaseModuleConfig> CreateConfigFromModule(const BaseFxModule& module) {
		if (module.GetName() == "GravityModule") {
			auto& mod = static_cast<const GravityModule&>(module);
			auto config = std::make_unique<GravityModuleConfig>();
			config->enabled = mod.IsEnabled();
			config->gravity = mod.GetGravity();
			return config;
		} else if (module.GetName() == "SizeOverLifetimeModule") {
			auto& mod = static_cast<const SizeOverLiftimeModule&>(module);
			auto config = std::make_unique<SizeOverLifetimeConfig>();
			config->enabled = mod.IsEnabled();
			config->isGrowing = mod.GetIsGrowing();
			config->easeType = mod.GetEaseType();
			return config;
		}
		return nullptr;
	}
}
