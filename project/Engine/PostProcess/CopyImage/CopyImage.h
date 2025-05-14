#pragma once
#include <Engine/PostProcess/Interface/IPostEffectPass.h>
#include <Engine/Graphics/Pipeline/Manager/PipelineStateManager.h>

class CopyImageEffect :
	public IPostEffectPass {
public:
	void Initialize(PipelineStateManager* pipelineMgr);
	void Apply(ID3D12GraphicsCommandList* cmd,
			   D3D12_GPU_DESCRIPTOR_HANDLE inputSRV,
			   IRenderTarget* outputRT) override;
	const std::string GetName() const override { return "CopyImage"; }

private:
	ComPtr<ID3D12PipelineState> pso_;
	ComPtr<ID3D12RootSignature> rootSignature_;
};