#pragma once

// engine
#include <Engine/PostProcess/GrayScale/GrayScale.h>

// c++
#include <memory>

class PostProcessCollection{
public:
	PostProcessCollection() = default;
	~PostProcessCollection() = default;

	void Initialize(PipelineStateManager* pipelineMgr);

	GrayScaleEffect* GetGrayScale();

private:
	std::unique_ptr<GrayScaleEffect> grayScale_;
};