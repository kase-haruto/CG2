#include "PostProcessCollection.h"

#include <Engine/Graphics/Pipeline/Service/PipelineService.h>

void PostProcessCollection::Initialize(PipelineService* service){
	effects_.clear();
	effectNames_.clear();

	//===================================================================*/
	//		grayScale
	//===================================================================*/
	{
		PipelineSet set = service->GetPipelineSet(PipelineTag::PostProcess::GrayScale);
		auto effect = std::make_unique<GrayScaleEffect>();
		effect->Initialize(set);
		effectNames_.push_back(effect->GetName());
		effects_.push_back(std::move(effect));
	}

	//===================================================================*/
	//		RadialBlur
	//===================================================================*/
	{
		PipelineSet set = service->GetPipelineSet(PipelineTag::PostProcess::RadialBlur);
		auto effect = std::make_unique<RadialBlurEffect>();
		effect->Initialize(set);
		effectNames_.push_back(effect->GetName());
		effects_.push_back(std::move(effect));
	}

	//===================================================================*/
	//		ChromaticAberration
	//===================================================================*/
	{
		PipelineSet set = service->GetPipelineSet(PipelineTag::PostProcess::ChromaticAberration);
		auto effect = std::make_unique<ChromaticAberrationEffect>();
		effect->Initialize(set);
		effectNames_.push_back(effect->GetName());
		effects_.push_back(std::move(effect));
	}

	//===================================================================*/
	//		No Effects
	//===================================================================*/
	{
		PipelineSet set = service->GetPipelineSet(PipelineTag::PostProcess::CopyImage);
		auto effect = std::make_unique<CopyImageEffect>();
		effect->Initialize(set);
		effectNames_.push_back(effect->GetName());
		effects_.push_back(std::move(effect));
	}

}

