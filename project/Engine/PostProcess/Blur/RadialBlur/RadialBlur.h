#pragma once
/* ========================================================================
/* include space
/* ===================================================================== */
#include <Engine/Foundation/Math/Vector2.h>
#include <Engine/Graphics/Buffer/DxConstantBuffer.h>
#include <Engine/Graphics/Pipeline/Pso/PsoDetails.h>
#include <Engine/PostProcess/Interface/IPostEffectPass.h>


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
	void Initialize(const PipelineSet& psoSet);
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

	// pso ================================================================*/
	PipelineSet psoSet_;
};


