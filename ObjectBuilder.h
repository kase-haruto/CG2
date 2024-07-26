#pragma once
#include"GameObject.h"

class ObjectBuilder{
public:
	ObjectBuilder(DirectXCommon* dxCommon, ViewProjection* viewPro, PipelineStateManager* psManager);

	void CreateModel();

private:
};

