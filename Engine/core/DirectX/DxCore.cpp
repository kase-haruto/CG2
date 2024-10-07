#include "core/DirectX/DxCore.h"

#include <cassert>
#include <dxgidebug.h>

// DirectXライブラリのリンカー指示
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "dxcompiler.lib")

DxCore::~DxCore(){}

void DxCore::Initialize(WinApp* winApp, uint32_t width, uint32_t height){
    winApp_ = winApp;
    clientWidth_ = width;
    clientHeight_ = height;

    SetViewPortAndScissor(width, height);

    //deviceの初期化
    dxDevice_.Initialize();
    //コマンド関連初期化
    dxCommand_.Initialize(dxDevice_.GetDevice());
    //swapChain初期化
    dxSwapChain_.Initialize(dxDevice_.GetDXGIFactory(), dxCommand_.GetCommandQueue(), winApp_->GetHWND(), width, height);
    //renderTarget初期化
    renderTarget_.Initialize(dxDevice_.GetDevice(), dxSwapChain_.GetSwapChain());
    renderTarget_.CreateDepthBuffer(dxDevice_.GetDevice(), width, height);
    //fenceの生成
    dxFence_.Initialize(dxDevice_.GetDevice());
}

void DxCore::PreDraw(){
    // バックバッファのインデックスを取得
    UINT backBufferIndex = dxSwapChain_.GetCurrentBackBufferIndex();

    // コマンドリストを取得
    ComPtr<ID3D12GraphicsCommandList> commandList = dxCommand_.GetCommandList();

    // バックバッファの状態を PRESENT から RENDER TARGET に変更
    D3D12_RESOURCE_BARRIER barrier = {};
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    barrier.Transition.pResource = dxSwapChain_.GetBackBuffer(backBufferIndex).Get();
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    commandList->ResourceBarrier(1, &barrier);

    // RenderTarget クラスを使用してクリア処理
    renderTarget_.ClearRenderTarget(commandList, backBufferIndex);

    // ビューポートとシザー矩形の設定
    commandList->RSSetViewports(1, &viewport_);
    commandList->RSSetScissorRects(1, &scissorRect_);
}


void DxCore::PostDraw(){
    HRESULT hr;
    // バックバッファのインデックスを取得
    UINT backBufferIndex = dxSwapChain_.GetCurrentBackBufferIndex();

    // コマンドリストを取得
    ComPtr<ID3D12GraphicsCommandList> commandList = dxCommand_.GetCommandList();

	//TransitionBarrierの設定
	D3D12_RESOURCE_BARRIER barrier {};
	//今回のバリアはTransition
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	//noneにしておく
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	//バリアを張る対象のリソース、現行のバッグバッファに対して行う
	barrier.Transition.pResource = dxSwapChain_.GetBackBuffer(backBufferIndex).Get();
	//遷移前(現行)のResourceState
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	//遷移後のResourceState
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	//transitionBarrierを張る
	commandList->ResourceBarrier(1, &barrier);

	hr = commandList->Close();
	assert(SUCCEEDED(hr));

	//コマンドリストの実行
	ID3D12CommandList* commandLists[] = {commandList.Get()};
    dxCommand_.GetCommandQueue()->ExecuteCommandLists(_countof(commandLists), commandLists);

	//gpuとosに画面の交換を行うように通知
    dxSwapChain_.Presetn();

	//Fenceの値を更新
    dxFence_.Signal(dxCommand_.GetCommandQueue());

	//fenceの値が指定したsignalの値にたどり着いてるか確認する
	dxFence_.Wait();

	//次のフレーム用のコマンドリストを準備
	dxCommand_.Reset();
}

void DxCore::SetViewPortAndScissor(uint32_t width, uint32_t height){
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
