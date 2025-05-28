#pragma once
/* ========================================================================
/* include space
/* ===================================================================== */
#include <Engine/PostProcess/Interface/IPostEffectPass.h>
#include <Engine/Graphics/Pipeline/Manager/PipelineStateManager.h>

#include <Engine/Graphics/Buffer/DxConstantBuffer.h>

#include <Engine/Foundation/Math/Vector2.h>

class RadialBlurEffect :
	public IPostEffectPass{
private:
	struct BlurParameter{
		Vector2 center;
		float width;
	};
	//===================================================================*/
	//		public methods
	//===================================================================*/
public:
	void Initialize(PipelineStateManager* pipelineMgr);
	void Apply(ID3D12GraphicsCommandList* cmd,
			   D3D12_GPU_DESCRIPTOR_HANDLE inputSRV,
			   IRenderTarget* outputRT) override;
	const std::string GetName() const override{ return "RadialBlur"; }

	//===================================================================*/
	//		private methods
	//===================================================================*/
private:
	BlurParameter blurParam_;
	DxConstantBuffer<BlurParameter> blurBuffer_;
	ComPtr<ID3D12PipelineState> pso_;
	ComPtr<ID3D12RootSignature> rootSignature_;
};


