#pragma once
#include <Engine/PostProcess/Interface/IPostEffectPass.h>
#include <Engine/core/DirectX/GpuResource/DxGpuResource.h>
#include <vector>

class PostEffectGraph{
public:
	void AddPass(IPostEffectPass* pass);
	void Execute(ID3D12GraphicsCommandList* cmd,
				 DxGpuResource* input,
				 IRenderTarget* finalTarget);

private:
	std::vector<IPostEffectPass*> passes_;
};
