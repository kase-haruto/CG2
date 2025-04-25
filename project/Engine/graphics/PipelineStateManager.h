#pragma once
#include "../graphics/PipelineState.h"

//engine
#include "pipeline/PipelineKey.h"

/* c++ */
#include<string>
#include<unordered_map>
//graphics関連
#include<d3d12.h>



class PipelineStateManager{
private:
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public:
	PipelineStateManager(ComPtr<ID3D12Device> device, std::shared_ptr<ShaderManager> shaderManager)
		: device_(device), shaderManager_(shaderManager){}
	~PipelineStateManager();

	bool CreatePipelineState(
		PipelineType pipelineType,
		const std::wstring& vsPath,
		const std::wstring& psPath,
		const D3D12_ROOT_SIGNATURE_DESC& rootSignatureDesc,
		const D3D12_GRAPHICS_PIPELINE_STATE_DESC& psoDesc,
		BlendMode blendMode
	);

	void Finalize();

	const ComPtr<ID3D12PipelineState>& GetPipelineState(
		PipelineType pipelineType,
		BlendMode blendMode
	) const;

	const ComPtr<ID3D12RootSignature>& GetRootSignature(
		PipelineType pipelineType,
		BlendMode blendMode
	) const;

private:
	ComPtr<ID3D12Device>device_;
	std::shared_ptr<ShaderManager>shaderManager_;
	// ★ キーを (pipelineType, blendMode) にした Map
	std::unordered_map<PipelineKey, std::unique_ptr<PipelineState>> pipelineStates_;
};

