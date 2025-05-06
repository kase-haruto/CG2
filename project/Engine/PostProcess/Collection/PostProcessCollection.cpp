#include "PostProcessCollection.h"

void PostProcessCollection::Initialize(PipelineStateManager* pipelineMgr){
	grayScale_ = std::make_unique<GrayScaleEffect>();
	grayScale_->Initialize(pipelineMgr);

	radialBlur_ = std::make_unique<RadialBlurEffect>();
	radialBlur_->Initialize(pipelineMgr);

	copyImage_ = std::make_unique<CopyImageEffect>();
	copyImage_->Initialize(pipelineMgr);
}

GrayScaleEffect* PostProcessCollection::GetGrayScale(){
	return grayScale_.get();
}
