#pragma once
/* ========================================================================
/* include space
/* ===================================================================== */

// engine
#include <Engine/Objects/3D/Actor/SceneObject.h>
#include <Engine/Application/Effects/Particle/Emitter/FxEmitter.h>

// c++


class ParticleSystemObject
	:public SceneObject ,
	 public FxEmitter {
public:
	//===================================================================*/
	//					public func
	//===================================================================*/
	ParticleSystemObject(const std::string& name);
	ParticleSystemObject() = default;
	~ParticleSystemObject() = default;

	void Initialize() {}
	void Update() override;
	void ShowGui()override;

	void SetDrawEnable(bool isDrawEnable)override;

private:
	//===================================================================*/
	//					private func
	//===================================================================*/
};

