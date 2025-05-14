#pragma once
/* ========================================================================
/* include space
/* ===================================================================== */
#include "Engine/graphics/Pipeline/PipelineType.h"
#include "Engine/objects/SceneObject.h"
#include "Engine/objects/Transform.h" 
#include <Engine/Foundation/Math/Matrix4x4.h>

//c++
#include <wrl.h>

class ICamera
	:public SceneObject{
public:
	//==================================================================*//
	//			public functions
	//==================================================================*//
	virtual~ICamera() = default;
	ICamera() = default;

	virtual void Update() = 0;
	virtual void UpdateMatrix() = 0;
	virtual void ShowGui()override{}
	virtual void TransfarToGPU(){}  // GPUへ転送
	virtual void StartShake([[maybe_unused]] float duration, [[maybe_unused]] float intensity){};

};

