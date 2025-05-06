#pragma once
#include <Engine/PostProcess/Interface/IPostEffectPass.h>
#include <Engine/graphics/PipelineStateManager.h>

#include <Engine/core/DirectX/Buffer/DxConstantBuffer.h>

#include <lib/myMath/Vector2.h>

class RadialBlur :
    public IPostEffectPass{
private:
	struct BlurParameter{
		Vector2 center;
		float width;
	};

public:
	void Initialize(PipelineStateManager* pipelineMgr);
	void Apply(ID3D12GraphicsCommandList* cmd,
			   D3D12_GPU_DESCRIPTOR_HANDLE inputSRV,
			   IRenderTarget* outputRT) override;
	const std::string GetName() const override{ return "RadialBlur"; }

private:
	BlurParameter blurParam_;

	ComPtr<ID3D12PipelineState> pso_;
	ComPtr<ID3D12RootSignature> rootSignature_;
};


