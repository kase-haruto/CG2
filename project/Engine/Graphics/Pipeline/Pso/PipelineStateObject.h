#pragma once
/* ========================================================================
/* include space
/* ===================================================================== */
#include <Engine/Graphics/Pipeline/Pso/PsoDetails.h>

// lib
#include <d3d12.h>
#include <dxcapi.h>
#include <wrl.h>
#include <memory>

class PipelineStateObject {
public:
	PipelineStateObject() = default;

	void SetRootSignature(ID3D12RootSignature* root);
	bool Initialize(const D3D12_GRAPHICS_PIPELINE_STATE_DESC& desc);

	// アクセサ
	const PipelineSet& GetPipelineSet() const{
		return {pipelineState_.Get(), rootSignature_.Get()};
	}

	const Microsoft::WRL::ComPtr<ID3D12PipelineState>& GetPipelineState()  const { return pipelineState_; }
	const Microsoft::WRL::ComPtr<ID3D12RootSignature>& GetRootSignature()  const { return rootSignature_; }

	void SetShaderBlobs(
		Microsoft::WRL::ComPtr<IDxcBlob> vs,
		Microsoft::WRL::ComPtr<IDxcBlob> ps) {
		vsBlob_ = std::move(vs);
		psBlob_ = std::move(ps);
	}

	bool Reload(ID3D12Device* device);   // ここでは実装だけ宣言

private:
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState_;

	Microsoft::WRL::ComPtr<IDxcBlob> vsBlob_;
	Microsoft::WRL::ComPtr<IDxcBlob> psBlob_;
};