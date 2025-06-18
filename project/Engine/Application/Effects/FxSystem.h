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
	void AddEmitter(FxEmitter* emitter);
	void Update(float dt);
	const std::vector<FxEmitter*>& GetEmitters() const{ return emitters_; }
private:
	//===================================================================*/
	//					private variable
	//===================================================================*/
	std::vector<FxEmitter*> emitters_;
};