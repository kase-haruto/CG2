#include "FxModuleContainer.h"

#include <Engine/Application/Effects/Particle/Module/Velocity/GravityModule.h>
#include <Engine/Application/Effects/Particle/Module/Size/SizeOverLiftimeModule.h>

FxModuleContainer::FxModuleContainer(){
	modules_.clear();

	// モジュールの初期化
	modules_.emplace_back(std::make_unique<GravityModule>("GravityModule")); // 重力モジュール
	modules_.emplace_back(std::make_unique<SizeOverLiftimeModule>("SizeOverLifetimeModule")); // サイズ変化モジュール

}
