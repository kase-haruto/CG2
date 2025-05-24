#pragma once

#include <Engine/Graphics/Pipeline/Shader/ShaderCompiler.h>
#include <Engine/Graphics/Pipeline/Factory/PsoFactory.h>
#include <Engine/Graphics/Pipeline/Library/PsoLibrary.h>

class PipelineService {
public:
	PipelineService();
	~PipelineService() = default;

	void RegisterAllPipelines();

	void Register(const GraphicsPipelineDesc& desc);

	ID3D12PipelineState* GetPipelineState(const GraphicsPipelineDesc& desc) { return library_->GetOrCreate(desc); }
	ID3D12RootSignature* GetRootSig(const GraphicsPipelineDesc& desc) { return library_->GetRoot(desc); }

	ShaderCompiler* GetCompiler()  const { return compiler_.get(); }
	PsoFactory* GetFactory()  const { return factory_.get(); }
	PsoLibrary* GetLibrary()  const { return library_.get(); }

private:
	std::unique_ptr<ShaderCompiler>   compiler_;
	std::unique_ptr<PsoFactory>  factory_;
	std::unique_ptr<PsoLibrary>  library_;
};
