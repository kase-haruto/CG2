#pragma once
/* ========================================================================
/*	include space
/* ===================================================================== */
#include <Engine/Application/Effects/Particle/Emitter/FxEmitter.h>

#include <memory>

struct FxUnit;

/* ========================================================================
/*	effect system
/* ===================================================================== */
class FxSystem {
public:
	//===================================================================*/
	//					public func
	//===================================================================*/
	void AddEmitter(std::shared_ptr<FxEmitter> emitter);
	void Update(float dt);
	std::vector<FxUnit> CollectAliveUnits() const;

private:
	//===================================================================*/
	//					private variable
	//===================================================================*/
	std::vector<std::shared_ptr<FxEmitter>> emitters_;
};