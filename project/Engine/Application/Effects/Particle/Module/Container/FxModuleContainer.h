#pragma once
/* ========================================================================
/* include space
/* ===================================================================== */

// engine
#include <Engine/Application/Effects/Particle/Module/BaseFxModule.h>

// c++
#include <vector>
#include <memory>

class FxModuleContainer{
public:
	//===================================================================*/
	//					public methods
	//===================================================================*/
	FxModuleContainer();
	~FxModuleContainer() = default;

	const std::vector<std::unique_ptr<BaseFxModule>>& GetModules() const{ return modules_; }

private:
	//===================================================================*/
	//					private methods
	//===================================================================*/
	std::vector<std::unique_ptr<BaseFxModule>> modules_;
};

