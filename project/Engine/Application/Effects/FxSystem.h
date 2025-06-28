#pragma once
/* ========================================================================
/*	include space
/* ===================================================================== */
// engine
#include <Engine/Application/Effects/Particle/Emitter/FxEmitter.h>
#include <Engine/Objects/ConfigurableObject/ConfigurableObject.h>
#include <Engine/Renderer/Particle/ParticleRenderer.h>

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
	FxSystem();
	~FxSystem() ;
	void AddEmitter(FxEmitter* emitter);
	void RemoveEmitter(FxEmitter* emitter);
	void Update();
	void Render(class PipelineService*, ID3D12GraphicsCommandList*);
	void Clear();
	const std::vector<FxEmitter*>& GetEmitters() const{ return emitters_; }
private:
	//===================================================================*/
	//					private variable
	//===================================================================*/
	std::vector<FxEmitter*> emitters_;
	std::unique_ptr<ParticleRenderer> particleRenderer_ ;
};