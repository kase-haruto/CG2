#include "CopyImage.h"

#include <Engine/PostProcess/FullscreenDrawer.h>

void CopyImageEffect::Initialize(PipelineStateManager* pipelineMgr) {
	pso_ = pipelineMgr->GetPipelineState(PipelineType::copyImage, BlendMode::NONE);
	rootSignature_ = pipelineMgr->GetRootSignature(PipelineType::copyImage, BlendMode::NONE);
}

void CopyImageEffect::Apply(ID3D12GraphicsCommandList* cmd,
							D3D12_GPU_DESCRIPTOR_HANDLE inputSRV,
							IRenderTarget* outputRT) {
	outputRT->GetResource()->Transition(cmd, D3D12_RESOURCE_STATE_RENDER_TARGET);

	outputRT->SetRenderTarget(cmd);

	cmd->SetPipelineState(pso_.Get());
	cmd->SetGraphicsRootSignature(rootSignature_.Get());
	cmd->SetGraphicsRootDescriptorTable(0, inputSRV);

	auto viewport = outputRT->GetViewport();
	auto scissor = outputRT->GetScissorRect();
	cmd->RSSetViewports(1, &viewport);
	cmd->RSSetScissorRects(1, &scissor);

	cmd->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	cmd->DrawInstanced(3, 1, 0, 0);
}
