#pragma once

#include <Engine/Application/Effects/Particle/Module/BaseFxModule.h>
#include <Data/Engine/Configs/Scene/Objects/Particle/EmitterConfig.h>
#include <Data/Engine/Configs/Scene/Objects/Particle/Module/ModuleConfig.h>

#include <vector>
#include <memory>
#include <string>
#include <algorithm>

class FxModuleContainer {
public:
	FxModuleContainer() = default;

	// 修正点: unique_ptr を受け取るよう変更
	FxModuleContainer(const std::vector<std::unique_ptr<BaseModuleConfig>>& moduleConfigs);

	void AddModuleByName(const std::string& name, bool enabled = true);
	void RemoveModuleByName(const std::string& name);
	bool HasModule(const std::string& name) const;

	const std::vector<std::unique_ptr<BaseFxModule>>& GetModules() const { return modules_; }

private:
	std::vector<std::unique_ptr<BaseFxModule>> modules_;
};
