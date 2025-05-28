#include "PostProcessCollection.h"

#include <Engine/Graphics/Pipeline/Manager/PipelineStateManager.h>

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
	PipelineSet radialBlurSet = pipelineMgr->GetPipelineSet(PipelineType::RadialBlur, BlendMode::NONE);
	radialBlur_ = std::make_unique<RadialBlurEffect>();
	radialBlur_->Initialize(radialBlurSet);

	//===================================================================*/
	//		No Effects
	//===================================================================*/
	PipelineSet copyImageSet = pipelineMgr->GetPipelineSet(PipelineType::copyImage, BlendMode::NONE);
	copyImage_ = std::make_unique<CopyImageEffect>();
	copyImage_->Initialize(copyImageSet);
}

GrayScaleEffect* PostProcessCollection::GetGrayScale(){
	return grayScale_.get();
}
