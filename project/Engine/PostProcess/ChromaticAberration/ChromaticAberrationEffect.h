#pragma once
/* ========================================================================
/* include space
/* ===================================================================== */
#include <Engine/PostProcess/Interface/IPostEffectPass.h>
#include <Engine/Graphics/Pipeline/Pso/PsoDetails.h>
#include <Engine/Graphics/Buffer/DxConstantBuffer.h>

struct ChromaticAberrationSettings{
	float intensity = 0.2f;
};

class ChromaticAberrationEffect :
	public IPostEffectPass{
public:
	void Initialize(const PipelineSet& psoSet);
	void Apply(ID3D12GraphicsCommandList* cmd,
			   D3D12_GPU_DESCRIPTOR_HANDLE inputSRV,
			   IRenderTarget* outputRT) override;
	const std::string GetName() const override{ return "ChromaticAberration"; }

private:
	PipelineSet psoSet_;

	//buffer
	DxConstantBuffer<ChromaticAberrationSettings> settingsBuffer_;
};

