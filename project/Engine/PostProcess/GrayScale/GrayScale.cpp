#include "GrayScale.h"
#include <Engine/PostProcess/FullscreenDrawer.h>

void GrayScaleEffect::Initialize(PipelineStateManager* pipelineMgr){
	pso_ = pipelineMgr->GetPipelineState(PipelineType::GrayScale, BlendMode::NONE);
	rootSignature_ = pipelineMgr->GetRootSignature(PipelineType::GrayScale, BlendMode::NONE);
}

void GrayScaleEffect::Apply(ID3D12GraphicsCommandList* cmd,
							D3D12_GPU_DESCRIPTOR_HANDLE inputSRV,
							IRenderTarget* outputRT){
	outputRT->SetRenderTarget(cmd);

	cmd->SetPipelineState(pso_.Get());
	cmd->SetGraphicsRootSignature(rootSignature_.Get());
	cmd->SetGraphicsRootDescriptorTable(0, inputSRV);

	auto viewport = outputRT->GetViewport();
	auto scissor = outputRT->GetScissorRect();
	cmd->DrawInstanced(3, 1, 0, 0);
}
