#include "FxModuleContainer.h"

#include <Engine/Application/Effects/Particle/Module/Velocity/GravityModule.h>
#include <Engine/Application/Effects/Particle/Module/Size/SizeOverLiftimeModule.h>
#include <Engine/Application/Effects/Particle/Module/Factory/ModuleFactory.h>

// 修正点: unique_ptr の vector を受け取る
FxModuleContainer::FxModuleContainer(const std::vector<std::unique_ptr<BaseModuleConfig>>& moduleConfigs) {
	for (const auto& cfg : moduleConfigs) {
		if (!cfg) continue;
		auto mod = FxModuleFactory::CreateFromConfig(*cfg);
		if (mod) {
			modules_.emplace_back(std::move(mod));
		}
	}
}

void FxModuleContainer::AddModuleByName(const std::string& name, bool enabled) {
	if (HasModule(name)) return;

	std::unique_ptr<BaseFxModule> module;

	if (name == "GravityModule") {
		module = std::make_unique<GravityModule>(name);
	} else if (name == "SizeOverLifetimeModule") {
		module = std::make_unique<SizeOverLiftimeModule>(name);
	}

	if (module) {
		module->SetEnabled(enabled);
		modules_.emplace_back(std::move(module));
	}
}

void FxModuleContainer::RemoveModuleByName(const std::string& name) {
	modules_.erase(
		std::remove_if(modules_.begin(), modules_.end(),
					   [&](const std::unique_ptr<BaseFxModule>& m) { return m->GetName() == name; }),
		modules_.end());
}

bool FxModuleContainer::HasModule(const std::string& name) const {
	for (const auto& m : modules_) {
		if (m->GetName() == name) return true;
	}
	return false;
}
