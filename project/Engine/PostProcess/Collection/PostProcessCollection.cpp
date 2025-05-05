#include "PostProcessCollection.h"

void PostProcessCollection::Initialize(PipelineStateManager* pipelineMgr){
	grayScale_ = std::make_unique<GrayScaleEffect>();
	grayScale_->Initialize(pipelineMgr);
}

GrayScaleEffect* PostProcessCollection::GetGrayScale(){
	return grayScale_.get();
}
