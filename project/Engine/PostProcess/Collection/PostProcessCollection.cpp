#include "PostProcessCollection.h"

#include <Engine/Graphics/Pipeline/Service/PipelineService.h>

void PostProcessCollection::Initialize(PipelineService* service){
	//===================================================================*/
	//		grayScale
	//===================================================================*/
	PipelineSet grayScaleSet = service->GetPipelineSet(PipelineTag::PostProcess::GrayScale);
	grayScale_ = std::make_unique<GrayScaleEffect>();
	grayScale_->Initialize(grayScaleSet);

	//===================================================================*/
	//		RadialBlur
	//===================================================================*/
	PipelineSet radialBlurSet = service->GetPipelineSet(PipelineTag::PostProcess::RadialBlur);
	radialBlur_ = std::make_unique<RadialBlurEffect>();
	radialBlur_->Initialize(radialBlurSet);

	//===================================================================*/
	//		ChromaticAberration
	//===================================================================*/
	PipelineSet chromaticAberrationSet = service->GetPipelineSet(PipelineTag::PostProcess::ChromaticAberration);
	chromaticAberration_ = std::make_unique<ChromaticAberrationEffect>();
	chromaticAberration_->Initialize(chromaticAberrationSet);

	//===================================================================*/
	//		No Effects
	//===================================================================*/
	PipelineSet copyImageSet = service->GetPipelineSet(PipelineTag::PostProcess::CopyImage);
	copyImage_ = std::make_unique<CopyImageEffect>();
	copyImage_->Initialize(copyImageSet);
}

GrayScaleEffect* PostProcessCollection::GetGrayScale(){
	return grayScale_.get();
}
