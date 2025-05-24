#include "PipelineService.h"

#include <Engine/Graphics/Pipeline/Presets/PipelinePresets.h>

PipelineService::PipelineService() {
	compiler_ = std::make_unique<ShaderCompiler>();
	compiler_->InitializeDXC();
	factory_ = std::make_unique<PsoFactory>(compiler_.get());
	library_ = std::make_unique<PsoLibrary>(factory_.get());
}

void PipelineService::RegisterAllPipelines() {

	//===================================================================*/
	//                    Object3D Pipelines
	//===================================================================*/
	for (int i = 0; i < static_cast<int>(BlendMode::kBlendModeCount); ++i) {
		BlendMode mode = static_cast<BlendMode>(i);

		GraphicsPipelineDesc desc = PipelinePresets::MakeObject3D(mode);

		Register(desc);
	}

	//===================================================================*/
	//                    SkinObject3D Pipelines
	//===================================================================*/
}

void PipelineService::Register(const GraphicsPipelineDesc& desc) {
	library_->GetOrCreate(desc);
}
