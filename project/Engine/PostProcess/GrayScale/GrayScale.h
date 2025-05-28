#pragma once

#include <Engine/PostProcess/Interface/IPostEffectPass.h>
#include <Engine/Graphics/Pipeline/Manager/PipelineStateManager.h>

struct PipelineSet;

class GrayScaleEffect
	: public IPostEffectPass{
public:
	void Initialize(PipelineSet psoSet);
	void Apply(ID3D12GraphicsCommandList* cmd,
			   D3D12_GPU_DESCRIPTOR_HANDLE inputSRV,
			   IRenderTarget* outputRT) override;

	const std::string GetName() const override{ return "GrayScale"; }

private:
	PipelineSet psoSet_;
};


