#pragma once

#include "Engine/graphics/Pipeline/PipelineType.h"

#include "Engine/objects/SceneObject.h"
#include "lib/myMath/Matrix4x4.h"
#include "Engine/objects/Transform.h" 

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

