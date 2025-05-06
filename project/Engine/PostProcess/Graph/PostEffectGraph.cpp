#include "PostEffectGraph.h"
#include <Engine/core/DirectX/RenderTarget/IRenderTarget.h>

void PostEffectGraph::AddPass(IPostEffectPass* pass){
	passes_.push_back(pass);
}

void PostEffectGraph::Execute(ID3D12GraphicsCommandList* cmd,
							  DxGpuResource* input,
							  IRenderTarget* finalTarget){
	input->Transition(cmd, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	D3D12_GPU_DESCRIPTOR_HANDLE currentInput = input->GetSRVGpuHandle();
	IRenderTarget* currentOutput = finalTarget;

	for (auto* pass : passes_){
		pass->Apply(cmd, currentInput, currentOutput);

		// 🔺 次のパス用に SRV 状態に戻す（Apply内ではRENDER_TARGETになっているので）
		currentOutput->GetResource()->Transition(cmd, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

		currentInput = currentOutput->GetSRV();
	}

}
