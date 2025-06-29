#pragma once

#include <Engine/Application/Effects/Particle/Module/BaseFxModule.h>
#include <Engine/Application/Effects/Particle/Module/Velocity/GravityModule.h>
#include <Engine/Application/Effects/Particle/Module/Size/SizeOverLiftimeModule.h>
#include <Data/Engine/Configs/Scene/Objects/Particle/Module/ModuleConfig.h>

#include <memory>
#include <string>

namespace FxModuleFactory {
	std::unique_ptr<BaseFxModule> CreateFromConfig(const BaseModuleConfig& config);
	std::unique_ptr<BaseModuleConfig> CreateConfigFromModule(const BaseFxModule& module);
}