#include "engine/core/DirectX/DxCore.h"

/* lib */
#include <cassert>
#include <dxgidebug.h>
#include <d3dx12.h>

// DirectXライブラリのリンカー指示
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "dxcompiler.lib")

DxCore::~DxCore() {}

void DxCore::Initialize(WinApp* winApp, uint32_t width, uint32_t height) {
    winApp_ = winApp;
    clientWidth_ = width;
    clientHeight_ = height;

    SetViewPortAndScissor(width, height);

    // dxDevice, dxCommand, dxSwapChain, renderTarget, dxFence のユニークポインタを生成
    dxDevice_ = std::make_unique<DxDevice>();
    dxCommand_ = std::make_unique<DxCommand>();
    dxSwapChain_ = std::make_unique<DxSwapChain>();
    renderTarget_ = std::make_unique<RenderTarget>();
    dxFence_ = std::make_unique<DxFence>();

    // deviceの初期化
    dxDevice_->Initialize();
    // コマンド関連初期化
    dxCommand_->Initialize(dxDevice_->GetDevice());
    // swapChain初期化
    dxSwapChain_->Initialize(dxDevice_->GetDXGIFactory(), dxCommand_->GetCommandQueue(), winApp_->GetHWND(), width, height);
    // renderTarget初期化
    renderTarget_->Initialize(dxDevice_->GetDevice(), *dxSwapChain_);
    renderTarget_->CreateDepthBuffer(dxDevice_->GetDevice(), width, height);
    renderTarget_->CreateOffscreenRenderTarget(dxDevice_->GetDevice(), width, height);
    // fenceの生成
    dxFence_->Initialize(dxDevice_->GetDevice());
}

void DxCore::PreDraw(){
    UINT backBufferIndex = dxSwapChain_->GetCurrentBackBufferIndex();
    ComPtr<ID3D12GraphicsCommandList> commandList = dxCommand_->GetCommandList();

    // バックバッファの状態を RENDER_TARGET に遷移
    if (renderTarget_->backBufferStates_[backBufferIndex] != D3D12_RESOURCE_STATE_RENDER_TARGET){
        CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
            dxSwapChain_->GetBackBuffer(backBufferIndex).Get(),
            renderTarget_->backBufferStates_[backBufferIndex],
            D3D12_RESOURCE_STATE_RENDER_TARGET
        );
        commandList->ResourceBarrier(1, &barrier);

        // 状態を更新
        renderTarget_->backBufferStates_[backBufferIndex] = D3D12_RESOURCE_STATE_RENDER_TARGET;
    }

    // バックバッファを設定
    D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = renderTarget_->rtvHeap_->GetCPUDescriptorHandleForHeapStart();
    rtvHandle.ptr += backBufferIndex * renderTarget_->rtvDescriptorSize_;
    commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);

    // RenderTarget クラスを使用してクリア処理
    renderTarget_->ClearRenderTarget(commandList, backBufferIndex);

    // ビューポートとシザー矩形の設定
    commandList->RSSetViewports(1, &viewport_);
    commandList->RSSetScissorRects(1, &scissorRect_);
}


void DxCore::PreDrawOffscreen(){
    ComPtr<ID3D12GraphicsCommandList> commandList = dxCommand_->GetCommandList();

    // オフスクリーンターゲットの状態を RENDER_TARGET に遷移
    if (renderTarget_->offscreenRenderTargetState_ != D3D12_RESOURCE_STATE_RENDER_TARGET){
        CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
            renderTarget_->offscreenRenderTarget_.Get(),
            renderTarget_->offscreenRenderTargetState_,
            D3D12_RESOURCE_STATE_RENDER_TARGET
        );
        commandList->ResourceBarrier(1, &barrier);

        // 状態を更新
        renderTarget_->offscreenRenderTargetState_ = D3D12_RESOURCE_STATE_RENDER_TARGET;
    }

    // オフスクリーンターゲットを設定
    D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = renderTarget_->offscreenRtvHeap_->GetCPUDescriptorHandleForHeapStart();
    D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = renderTarget_->dsvHeap_->GetCPUDescriptorHandleForHeapStart(); // 深度ステンシルビュー
    commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, &dsvHandle);

    // クリアカラー設定
    float clearColor[] = {1.0f, 0.0f, 0.0f, 1.0f};
    commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
    commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

    // ビューポートとシザー矩形の設定
    commandList->RSSetViewports(1, &viewport_);
    commandList->RSSetScissorRects(1, &scissorRect_);
}

void DxCore::PostDraw(){
    UINT backBufferIndex = dxSwapChain_->GetCurrentBackBufferIndex();
    ComPtr<ID3D12GraphicsCommandList> commandList = dxCommand_->GetCommandList();

    // バックバッファの状態を RENDER_TARGET から PRESENT に変更
    if (renderTarget_->backBufferStates_[backBufferIndex] != D3D12_RESOURCE_STATE_PRESENT){
        CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
            dxSwapChain_->GetBackBuffer(backBufferIndex).Get(),
            renderTarget_->backBufferStates_[backBufferIndex],
            D3D12_RESOURCE_STATE_PRESENT
        );
        commandList->ResourceBarrier(1, &barrier);

        // 状態を更新
        renderTarget_->backBufferStates_[backBufferIndex] = D3D12_RESOURCE_STATE_PRESENT;
    }

    // コマンドリストを閉じる
    HRESULT hr = commandList->Close();
    assert(SUCCEEDED(hr));

    // コマンドリストの実行
    ID3D12CommandList* commandLists[] = {commandList.Get()};
    dxCommand_->GetCommandQueue()->ExecuteCommandLists(_countof(commandLists), commandLists);

    // 画面を更新
    dxSwapChain_->Presetn();

    // フェンスを使ってGPUとCPUの同期を行う
    dxFence_->Signal(dxCommand_->GetCommandQueue());
    dxFence_->Wait();

    // 次のフレーム用のコマンドリストを準備
    dxCommand_->Reset();
}


void DxCore::SetViewPortAndScissor(uint32_t width, uint32_t height) {
    //クライアント領域サイズと一緒にして画面に表示
    viewport_.Width = float(width);
    viewport_.Height = float(height);
    viewport_.TopLeftX = 0;
    viewport_.TopLeftY = 0;
    viewport_.MinDepth = 0.0f;
    viewport_.MaxDepth = 1.0f;

    //基本的にビューポートと同じ矩形が構成されるようにする
    scissorRect_.left = 0;
    scissorRect_.right = width;
    scissorRect_.top = 0;
    scissorRect_.bottom = height;
}
