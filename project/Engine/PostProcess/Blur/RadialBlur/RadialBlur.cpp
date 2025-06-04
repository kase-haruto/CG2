#include "RadialBlur.h"
#include <Engine/PostProcess/FullscreenDrawer.h>
#include <Engine/Graphics/Context/GraphicsGroup.h>

void RadialBlurEffect::Initialize(const PipelineSet& psoSet) {
	psoSet_ = psoSet;
	blurBuffer_.Initialize(GraphicsGroup::GetInstance()->GetDevice().Get());
}

void RadialBlurEffect::Apply(ID3D12GraphicsCommandList* cmd,
							D3D12_GPU_DESCRIPTOR_HANDLE inputSRV,
							IRenderTarget* outputRT) {
	outputRT->GetResource()->Transition(cmd, D3D12_RESOURCE_STATE_RENDER_TARGET);

	blurBuffer_.TransferData(blurParam_);
	outputRT->SetRenderTarget(cmd);

	psoSet_.SetCommand(cmd);
	cmd->SetGraphicsRootDescriptorTable(0, inputSRV);
	blurBuffer_.SetCommand(cmd, 1);

	cmd->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	cmd->DrawInstanced(3, 1, 0, 0);
}
