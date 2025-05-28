#include "PostProcessCollection.h"

void PostProcessCollection::Initialize(PipelineStateManager* pipelineMgr){

	//===================================================================*/
	//		grayScale
	//===================================================================*/
	PipelineSet grayScaleSet = pipelineMgr->GetPipelineSet(PipelineType::GrayScale, BlendMode::NONE);
	grayScale_ = std::make_unique<GrayScaleEffect>();
	grayScale_->Initialize(grayScaleSet);

	//===================================================================*/
	//		RadialBlur
	//===================================================================*/
	radialBlur_ = std::make_unique<RadialBlurEffect>();
	radialBlur_->Initialize(pipelineMgr);

	//===================================================================*/
	//		No Effects
	//===================================================================*/
	copyImage_ = std::make_unique<CopyImageEffect>();
	copyImage_->Initialize(pipelineMgr);
}

GrayScaleEffect* PostProcessCollection::GetGrayScale(){
	return grayScale_.get();
}
