#pragma once

// engine
#include <Engine/PostProcess/GrayScale/GrayScale.h>
#include <Engine/PostProcess/Blur/RadialBlur/RadialBlur.h>
#include <Engine/PostProcess/CopyImage/CopyImage.h>
#include <Engine/PostProcess/ChromaticAberration/ChromaticAberrationEffect.h>

// c++
#include <memory>

class PostProcessCollection{
public:
	PostProcessCollection() = default;
	~PostProcessCollection() = default;

	void Initialize(class PipelineService* pipelineMgr);

	IPostEffectPass* GetEffectByName(const std::string& name){
		for (const auto& effect : effects_){
			if (effect->GetName() == name){
				return effect.get();
			}
		}
		return nullptr;
	}


	const std::vector<std::string>& GetEffectNames() const{ return effectNames_; }

private:
	std::vector<std::unique_ptr<IPostEffectPass>> effects_;

	std::vector<std::string> effectNames_;
};