#include "ChromaticAberrationEffect.h"
#include <Engine/Graphics/Context/GraphicsGroup.h>

void ChromaticAberrationEffect::Initialize(const PipelineSet& psoSet) {
	psoSet_ = psoSet;
	settingsBuffer_.Initialize(GraphicsGroup::GetInstance()->GetDevice().Get());
}

void ChromaticAberrationEffect::Apply(ID3D12GraphicsCommandList* cmd,
									  D3D12_GPU_DESCRIPTOR_HANDLE inputSRV,
									  IRenderTarget* outputRT) {

	// 外部から渡された intensity_ を使用
	settingsBuffer_.TransferData(ChromaticAberrationSettings{ intensity_ });

	outputRT->GetResource()->Transition(cmd, D3D12_RESOURCE_STATE_RENDER_TARGET);
	outputRT->SetRenderTarget(cmd);
	psoSet_.SetCommand(cmd);

	cmd->SetGraphicsRootDescriptorTable(0, inputSRV);
	settingsBuffer_.SetCommand(cmd, 1);

	cmd->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	cmd->DrawInstanced(3, 1, 0, 0);
}