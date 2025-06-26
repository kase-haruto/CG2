#pragma once
/* ========================================================================
/*	include space
/* ===================================================================== */
// engine
#include <Engine/Application/Effects/Particle/Emitter/FxEmitter.h>

// c++ 
#include <memory>

/* ========================================================================
/*	effect system
/* ===================================================================== */
class FxSystem {
public:
	//===================================================================*/
	//					public func
	//===================================================================*/
	void AddEmitter(FxEmitter* emitter);
	void RemoveEmitter(FxEmitter* emitter);
	void Update();
	const std::vector<FxEmitter*>& GetEmitters() const{ return emitters_; }
private:
	//===================================================================*/
	//					private variable
	//===================================================================*/
	std::vector<FxEmitter*> emitters_;
};