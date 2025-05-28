#pragma once

// engine
#include <Engine/PostProcess/GrayScale/GrayScale.h>
#include <Engine/PostProcess/Blur/RadialBlur/RadialBlur.h>
#include <Engine/PostProcess/CopyImage/CopyImage.h>

// c++
#include <memory>

class PostProcessCollection{
public:
	PostProcessCollection() = default;
	~PostProcessCollection() = default;

	void Initialize(class PipelineStateManager* pipelineMgr);

	GrayScaleEffect* GetGrayScale();
	RadialBlurEffect* GetRadialBlur() { return radialBlur_.get(); }
	CopyImageEffect* GetCopyImage() { return copyImage_.get(); }

private:
	std::unique_ptr<GrayScaleEffect> grayScale_;
	std::unique_ptr<RadialBlurEffect> radialBlur_;
	std::unique_ptr<CopyImageEffect> copyImage_;
};