#pragma once
/* ========================================================================
/* include space
/* ===================================================================== */
// lib
#include <d3d12.h>
#include <wrl.h>
#include <memory>

class PipelineStateObject {
public:
    PipelineStateObject() = default;

    void SetRootSignature(ID3D12RootSignature* root);
    bool Initialize(const D3D12_GRAPHICS_PIPELINE_STATE_DESC& desc);

    // アクセサ
    const Microsoft::WRL::ComPtr<ID3D12PipelineState>& GetPipelineState()  const { return pipelineState_; }
    const Microsoft::WRL::ComPtr<ID3D12RootSignature>& GetRootSignature()  const { return rootSignature_; }

private:
    Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_;
    Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState_;
};