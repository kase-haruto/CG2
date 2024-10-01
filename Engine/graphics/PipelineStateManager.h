#pragma once
#include"PipelineState.h"

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

	bool CreatePipelineState(const PipelineType& name,
							const std::wstring& vsPath,const std::wstring& psPath,
							const D3D12_ROOT_SIGNATURE_DESC& rootSignatureDesc,
							const D3D12_GRAPHICS_PIPELINE_STATE_DESC& psoDesc);

	void Finalize();

	const ComPtr<ID3D12PipelineState>& GetPipelineState(const PipelineType& name)const;
	const ComPtr<ID3D12RootSignature>& GetRootSignature(const PipelineType& name)const;

private:
	ComPtr<ID3D12Device>device_;
	std::shared_ptr<ShaderManager>shaderManager_;
	std::unordered_map<PipelineType, std::unique_ptr<PipelineState>>pipelineStates_;
};

