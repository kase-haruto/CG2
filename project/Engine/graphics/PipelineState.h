#pragma once

// engine
#include "blendMode/BlendMode.h"

// lib
#include <d3d12.h>
#include <wrl.h>
#include <string>
#include <dxcapi.h>
#include <memory>

#include"ShaderManager.h"

using Microsoft::WRL::ComPtr;

class PipelineState{
public:
	PipelineState(ComPtr<ID3D12Device>device, std::shared_ptr<ShaderManager> shaderManager);
	~PipelineState();

	bool Initialize(const std::wstring& vsPath, const std::wstring& psPath,
					const D3D12_ROOT_SIGNATURE_DESC& rootSignatureDesc,
					const D3D12_GRAPHICS_PIPELINE_STATE_DESC& psoDesc,
					const BlendMode& blendMode);

	const ComPtr<ID3D12PipelineState>& GetPipelineState()const{ return pipelineState_; }
	const ComPtr<ID3D12RootSignature>& GetRootSignature()const{ return rootSignature_; }

private:
	/// <summary>
	/// グラフィック関連
	/// </summary>
	ComPtr<ID3D12Device>device_;
	ComPtr<ID3D12RootSignature>rootSignature_;
	ComPtr<ID3D12PipelineState>pipelineState_;

	/// <summary>
	/// shader
	/// </summary>
	ComPtr<IDxcBlob>vertexShader_;
	ComPtr<IDxcBlob>pixelShader_;
	std::shared_ptr<ShaderManager>shaderManager_;
};

