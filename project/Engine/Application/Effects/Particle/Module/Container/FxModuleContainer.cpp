#include "FxModuleContainer.h"

#include <Engine/Application/Effects/Particle/Module/Velocity/GravityModule.h>

FxModuleContainer::FxModuleContainer(){
	modules_.clear();

	// モジュールの初期化
	modules_.emplace_back(std::make_unique<GravityModule>("GravityModule")); // 重力モジュール
}
