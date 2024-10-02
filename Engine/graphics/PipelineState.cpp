#include "PipelineState.h"
#include"myFunc/ConvertString.h"
PipelineState::PipelineState(ComPtr<ID3D12Device>device, std::shared_ptr<ShaderManager> shaderManager)
    : device_(device), shaderManager_(shaderManager){}

PipelineState::~PipelineState(){
    device_.Reset();
    rootSignature_.Reset();
    pipelineState_.Reset();
}



bool PipelineState::Initialize(const std::wstring& vsPath, const std::wstring& psPath,
                               const D3D12_ROOT_SIGNATURE_DESC& rootSignatureDesc,
                               const D3D12_GRAPHICS_PIPELINE_STATE_DESC& psoDesc){
    vertexShader_ = shaderManager_->CompileShader(L"Engine/graphics/shaders/" + vsPath, L"vs_6_0");
    pixelShader_ = shaderManager_->CompileShader(L"Engine/graphics/shaders/" + psPath, L"ps_6_0");
    if (!vertexShader_ || !pixelShader_){
        Log("Failed to compile shaders.");
        return false;
    }

    // Root Signature の作成
    ComPtr<ID3DBlob> signature;
    ComPtr<ID3DBlob> error;
    HRESULT hr = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error);
    if (FAILED(hr)){
        if (error){
            OutputDebugStringA(( char* ) error->GetBufferPointer());
        }
        return false;
    }

    hr = device_->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&rootSignature_));
    if (FAILED(hr)){
        return false;
    }

    // PSO の設定
    D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc = psoDesc;
    pipelineDesc.pRootSignature = rootSignature_.Get();
    pipelineDesc.VS = {vertexShader_->GetBufferPointer(), vertexShader_->GetBufferSize()};
    pipelineDesc.PS = {pixelShader_->GetBufferPointer(), pixelShader_->GetBufferSize()};

    hr = device_->CreateGraphicsPipelineState(&pipelineDesc, IID_PPV_ARGS(&pipelineState_));
    return SUCCEEDED(hr);
}