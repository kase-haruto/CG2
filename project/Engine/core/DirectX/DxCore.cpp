#include "engine/core/DirectX/DxCore.h"
#include "Engine/graphics/GraphicsGroup.h"
#include <cassert>
#include <dxgidebug.h>
#include <d3dx12.h>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "dxcompiler.lib")

DxCore::~DxCore(){
    ReleaseResources();
}

void DxCore::ReleaseResources(){
    renderTarget_.reset();
    dxSwapChain_.reset();
    dxFence_.reset();
    dxCommand_.reset();
    dxDevice_.reset();
}

void DxCore::Initialize(WinApp* winApp, uint32_t width, uint32_t height){
    winApp_ = winApp;
    clientWidth_ = width;
    clientHeight_ = height;

    SetViewPortAndScissor(width, height);

    // 各種DirectX関連インスタンスの生成
    dxDevice_ = std::make_unique<DxDevice>();
    dxCommand_ = std::make_unique<DxCommand>();
    dxSwapChain_ = std::make_unique<DxSwapChain>();
    renderTarget_ = std::make_unique<RenderTarget>();
    dxFence_ = std::make_unique<DxFence>();

    // 初期化処理
    dxDevice_->Initialize();
    dxCommand_->Initialize(dxDevice_->GetDevice());
    dxSwapChain_->Initialize(dxDevice_->GetDXGIFactory(), dxCommand_->GetCommandQueue(), winApp_->GetHWND(), width, height);
    renderTarget_->Initialize(dxDevice_->GetDevice(), *dxSwapChain_);
    renderTarget_->CreateDepthBuffer(dxDevice_->GetDevice(), width, height);
    resourceStateTracker_.SetResourceState(dxSwapChain_->GetBackBuffer(0).Get(), D3D12_RESOURCE_STATE_PRESENT);
    resourceStateTracker_.SetResourceState(dxSwapChain_->GetBackBuffer(1).Get(), D3D12_RESOURCE_STATE_PRESENT);
    dxFence_->Initialize(dxDevice_->GetDevice());
}

void DxCore::RendererInitialize(uint32_t width, uint32_t height){
    renderTarget_->CreateOffscreenRenderTarget(dxDevice_->GetDevice(), width, height);
    resourceStateTracker_.SetResourceState(renderTarget_->offscreenRenderTarget_.Get(), D3D12_RESOURCE_STATE_RENDER_TARGET);
}

void DxCore::PreDraw(){
    UINT backBufferIndex = dxSwapChain_->GetCurrentBackBufferIndex();
    ComPtr<ID3D12GraphicsCommandList> commandList = dxCommand_->GetCommandList();

    TransitionResource(commandList.Get(), dxSwapChain_->GetBackBuffer(backBufferIndex).Get(), D3D12_RESOURCE_STATE_RENDER_TARGET);

    D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = renderTarget_->rtvHeap_->GetCPUDescriptorHandleForHeapStart();
    rtvHandle.ptr += backBufferIndex * renderTarget_->rtvDescriptorSize_;

    commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);
    renderTarget_->ClearRenderTarget(commandList, backBufferIndex);

    commandList->RSSetViewports(1, &viewport_);
    commandList->RSSetScissorRects(1, &scissorRect_);
}

void DxCore::PreDrawOffscreen(){
    ComPtr<ID3D12GraphicsCommandList> commandList = dxCommand_->GetCommandList();

    D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = renderTarget_->offscreenRtvHeap_->GetCPUDescriptorHandleForHeapStart();
    D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = renderTarget_->dsvHeap_->GetCPUDescriptorHandleForHeapStart();

    commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, &dsvHandle);

    float clearColor[] = {0.03f, 0.03f, 0.034f, 1.0f};
    commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
    commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

    commandList->RSSetViewports(1, &viewport_);
    commandList->RSSetScissorRects(1, &scissorRect_);
}

void DxCore::PostDraw(){
    UINT backBufferIndex = dxSwapChain_->GetCurrentBackBufferIndex();
    ComPtr<ID3D12GraphicsCommandList> commandList = dxCommand_->GetCommandList();

    // 状態遷移: オフスクリーン → Pixel Shader Resource
    TransitionResource(commandList.Get(), renderTarget_->offscreenRenderTarget_.Get(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

    // PSO と RootSignature の取得
    auto pipelineState = GraphicsGroup::GetInstance()->GetPipelineState(copyImage);
    auto rootSignature = GraphicsGroup::GetInstance()->GetRootSignature(copyImage);
    if (!pipelineState || !rootSignature){
        OutputDebugStringA("PipelineState or RootSignature is null.\n");
        return;
    }
    // トポロジーを設定する例 (三角形リスト)
    commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    commandList->SetGraphicsRootSignature(rootSignature.Get());
    commandList->SetPipelineState(pipelineState.Get());
    commandList->SetGraphicsRootDescriptorTable(0, renderTarget_->offscreenSrvGpuDescriptorHandle_);

    // 描画コマンド
    commandList->DrawInstanced(3, 1, 0, 0);

    // 状態遷移: オフスクリーン → Render Target
    TransitionResource(commandList.Get(), renderTarget_->offscreenRenderTarget_.Get(), D3D12_RESOURCE_STATE_RENDER_TARGET);

    // バックバッファ → Present
    TransitionResource(commandList.Get(), dxSwapChain_->GetBackBuffer(backBufferIndex).Get(), D3D12_RESOURCE_STATE_PRESENT);
 
    // コマンドリストの終了
    HRESULT hr = commandList->Close();
    if (FAILED(hr)){
        OutputDebugStringA("Failed to close command list.\n");
        assert(SUCCEEDED(hr));
    }

    // コマンドリストの実行
    ID3D12CommandList* commandLists[] = {commandList.Get()};
    dxCommand_->GetCommandQueue()->ExecuteCommandLists(_countof(commandLists), commandLists);

    // スワップチェインの更新
    dxSwapChain_->Presetn();

    // フェンス同期
    dxFence_->Signal(dxCommand_->GetCommandQueue());
    dxFence_->Wait();

    // コマンドリストのリセット
    dxCommand_->Reset();
}


void DxCore::SetViewPortAndScissor(uint32_t width, uint32_t height){
    viewport_ = {0.0f, 0.0f, static_cast< float >(width), static_cast< float >(height), 0.0f, 1.0f};
    scissorRect_ = {0, 0, static_cast< LONG >(width), static_cast< LONG >(height)};
}

void DxCore::TransitionResource(ID3D12GraphicsCommandList* commandList, ID3D12Resource* resource, D3D12_RESOURCE_STATES afterState){
    D3D12_RESOURCE_STATES beforeState = resourceStateTracker_.GetResourceState(resource);

    if (beforeState != afterState){
        D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(resource, beforeState, afterState);
        commandList->ResourceBarrier(1, &barrier);
        resourceStateTracker_.SetResourceState(resource, afterState);
    }
}
