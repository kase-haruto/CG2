// DxCore.cpp
#include "DxCore.h"

#include <Engine/Graphics/RenderTarget/OffscreenRT/OffscreenRenderTarget.h>
#include <Engine/Graphics/RenderTarget/SwapChainRT/SwapChainRenderTarget.h>
#include <Engine/Graphics/Context/GraphicsGroup.h>
#include <Engine/Application/System/Enviroment.h>
#include <Engine/PostProcess/FullscreenDrawer.h>

#include <cassert>
#include <dxgidebug.h>
#include <d3dx12.h>
#include <externals/imgui/imgui.h>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "dxcompiler.lib")

DxCore::~DxCore(){
	ReleaseResources();
}

void DxCore::ReleaseResources(){
	dxSwapChain_.reset();
	dxFence_.reset();
	dxCommand_.reset();
	dxDevice_.reset();
}

void DxCore::Initialize(WinApp* winApp, uint32_t width, uint32_t height){
	winApp_ = winApp;
	clientWidth_ = width;
	clientHeight_ = height;

	// 各種DirectX関連インスタンスの生成
	dxDevice_ = std::make_unique<DxDevice>();
	dxCommand_ = std::make_unique<DxCommand>();
	dxSwapChain_ = std::make_unique<DxSwapChain>();
	dxFence_ = std::make_unique<DxFence>();

	// 初期化処理
	dxDevice_->Initialize();
	dxCommand_->Initialize(dxDevice_->GetDevice());
	dxSwapChain_->Initialize(dxDevice_->GetDXGIFactory(), dxCommand_->GetCommandQueue(), winApp_->GetHWND(), width, height);
	dxFence_->Initialize(dxDevice_->GetDevice());
}

void DxCore::RendererInitialize(uint32_t width, uint32_t height){
	renderTargetCollection_ = std::make_unique<RenderTargetCollection>();
	auto device = dxDevice_->GetDevice();

	// RTV heap（SwapChain用に2つ、Offscreen用に1つ、PostEffect用に1つ、Debug用に1つ、Ping-Pong用に2つ = 合計7つ）
	D3D12_DESCRIPTOR_HEAP_DESC rtvDesc = {};
	rtvDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvDesc.NumDescriptors = 7;
	rtvDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	device->CreateDescriptorHeap(&rtvDesc, IID_PPV_ARGS(&rtvHeap_));
	rtvDescriptorSize_ = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	// DSV heap（1つ）
	D3D12_DESCRIPTOR_HEAP_DESC dsvDesc = {};
	dsvDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvDesc.NumDescriptors = 1;
	dsvDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	device->CreateDescriptorHeap(&dsvDesc, IID_PPV_ARGS(&dsvHeap_));

	// 各種ハンドル取得
	D3D12_CPU_DESCRIPTOR_HANDLE baseRTVHandle = rtvHeap_->GetCPUDescriptorHandleForHeapStart();
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = dsvHeap_->GetCPUDescriptorHandleForHeapStart();

	// SwapChain バックバッファを取得して RTV 作成
	for (UINT i = 0; i < 2; ++i){
		ComPtr<ID3D12Resource> backBuffer;
		dxSwapChain_->GetSwapChain()->GetBuffer(i, IID_PPV_ARGS(&backBuffer));
		D3D12_CPU_DESCRIPTOR_HANDLE handle = baseRTVHandle;
		handle.ptr += i * rtvDescriptorSize_;
		device->CreateRenderTargetView(backBuffer.Get(), nullptr, handle);
		dxSwapChain_->SetBackBuffer(i, backBuffer); // バッファ保持
	}

	auto swapchainRT = std::make_unique<SwapChainRenderTarget>();
	swapchainRT->Initialize(dxSwapChain_.get(), rtvHeap_.Get(), rtvDescriptorSize_);
	renderTargetCollection_->Add(RenderTargetType::BackBuffer,"BackBuffer", std::move(swapchainRT));

	// Offscreen はスロット2を使用
	D3D12_CPU_DESCRIPTOR_HANDLE offscreenRTVHandle = baseRTVHandle;
	offscreenRTVHandle.ptr += rtvDescriptorSize_ * 2;

	auto offscreenRT = std::make_unique<OffscreenRenderTarget>();
	offscreenRT->Initialize(device.Get(), width, height, format_, offscreenRTVHandle, dsvHandle);
	renderTargetCollection_->Add(RenderTargetType::Main,"Offscreen", std::move(offscreenRT));

	D3D12_CPU_DESCRIPTOR_HANDLE postEffectRTVHandle = baseRTVHandle;
	postEffectRTVHandle.ptr += rtvDescriptorSize_ * 3;
	auto postEffectRT = std::make_unique<OffscreenRenderTarget>();
	postEffectRT->Initialize(device.Get(), width, height, format_, /*RTV*/ postEffectRTVHandle, /*DSV*/ dsvHandle);
	renderTargetCollection_->Add(RenderTargetType::PostEffectOutput,"PostEffectOutput", std::move(postEffectRT));


	// debug
	D3D12_CPU_DESCRIPTOR_HANDLE debugRTVHandle = baseRTVHandle;
	debugRTVHandle.ptr += rtvDescriptorSize_ * 4;

	auto debugRT = std::make_unique<OffscreenRenderTarget>();
	debugRT->Initialize(device.Get(), width, height, format_, debugRTVHandle, dsvHandle);

	renderTargetCollection_->Add(RenderTargetType::DebugView,"DebugView", std::move(debugRT));

	// postEffect切り替え用
	// PostEffectBuffer1（スロット5として）
	D3D12_CPU_DESCRIPTOR_HANDLE postEffectBuffer1Handle = baseRTVHandle;
	postEffectBuffer1Handle.ptr += rtvDescriptorSize_ * 5;

	auto postEffectBuffer1 = std::make_unique<OffscreenRenderTarget>();
	postEffectBuffer1->Initialize(device.Get(), width, height, format_, postEffectBuffer1Handle, dsvHandle);
	renderTargetCollection_->Add(RenderTargetType::PostEffectBuffer1,"PostEffectBuffer1", std::move(postEffectBuffer1));

	// PostEffectBuffer2（スロット6として）
	D3D12_CPU_DESCRIPTOR_HANDLE postEffectBuffer2Handle = baseRTVHandle;
	postEffectBuffer2Handle.ptr += rtvDescriptorSize_ * 6;

	auto postEffectBuffer2 = std::make_unique<OffscreenRenderTarget>();
	postEffectBuffer2->Initialize(device.Get(), width, height, format_, postEffectBuffer2Handle, dsvHandle);
	renderTargetCollection_->Add(RenderTargetType::PostEffectBuffer2,"PostEffectBuffer2", std::move(postEffectBuffer2));
}


void DxCore::PreDraw(){
	UINT bufferIndex = dxSwapChain_->GetCurrentBackBufferIndex();
	auto commandList = dxCommand_->GetCommandList();

	// SwapChainRenderTarget の設定
	if (auto* swapchainRT = dynamic_cast< SwapChainRenderTarget* >(renderTargetCollection_->Get("BackBuffer"))){
		swapchainRT->SetBufferIndex(bufferIndex);
		swapchainRT->TransitionTo(commandList.Get(), D3D12_RESOURCE_STATE_RENDER_TARGET);
		swapchainRT->Clear(commandList.Get());
		swapchainRT->SetRenderTarget(commandList.Get());
	}

}

void DxCore::PreDrawOffscreen(){
	auto* offscreenTarget = renderTargetCollection_->Get("Offscreen");
	if (offscreenTarget){
		offscreenTarget->Clear(dxCommand_->GetCommandList().Get());
		offscreenTarget->TransitionTo(dxCommand_->GetCommandList().Get(), D3D12_RESOURCE_STATE_RENDER_TARGET);
		offscreenTarget->SetRenderTarget(dxCommand_->GetCommandList().Get());
	}
}

void DxCore::DrawOffscreenTexture(){
	auto commandList = dxCommand_->GetCommandList();

	// SwapChainRenderTarget の設定
	auto* backBufferTarget = renderTargetCollection_->Get("BackBuffer");
	if (auto* swapchainRT = dynamic_cast< SwapChainRenderTarget* >(backBufferTarget)){
		UINT bufferIndex = dxSwapChain_->GetCurrentBackBufferIndex();
		swapchainRT->SetBufferIndex(bufferIndex);
		swapchainRT->TransitionTo(commandList.Get(), D3D12_RESOURCE_STATE_RENDER_TARGET);
		swapchainRT->SetRenderTarget(commandList.Get());
	}

	// Offscreen を PixelShaderResource に遷移
	auto* offscreenTarget = renderTargetCollection_->Get("Offscreen");
	if (offscreenTarget){
		offscreenTarget->TransitionTo(commandList.Get(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

		auto pipelineState = GraphicsGroup::GetInstance()->GetPipelineState(GrayScale, BlendMode::NONE);
		auto rootSignature = GraphicsGroup::GetInstance()->GetRootSignature(GrayScale, BlendMode::NONE);

		commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		commandList->SetGraphicsRootSignature(rootSignature.Get());
		commandList->SetPipelineState(pipelineState.Get());
		commandList->SetGraphicsRootDescriptorTable(0, offscreenTarget->GetSRV());
		commandList->DrawInstanced(3, 1, 0, 0);
	}
}


void DxCore::PostDraw(){
	UINT bufferIndex = dxSwapChain_->GetCurrentBackBufferIndex();
	auto commandList = dxCommand_->GetCommandList();

	if (auto* swapchainRT = dynamic_cast< SwapChainRenderTarget* >(renderTargetCollection_->Get("BackBuffer"))){
		swapchainRT->SetBufferIndex(bufferIndex);
		swapchainRT->TransitionTo(commandList.Get(), D3D12_RESOURCE_STATE_PRESENT);
	}

	HRESULT hr = commandList->Close();
	if (FAILED(hr)){
		// ログを出力するか、デバッグ用にブレークポイントを設定
		OutputDebugStringA("commandList->Close() failed.\n");

		// エラーハンドリング
		throw std::runtime_error("Failed to close command list.");
	}

	ID3D12CommandList* commandLists[] = {commandList.Get()};
	dxCommand_->GetCommandQueue()->ExecuteCommandLists(_countof(commandLists), commandLists);

	dxSwapChain_->Present();
	dxFence_->Signal(dxCommand_->GetCommandQueue());
	dxFence_->Wait();
	dxCommand_->Reset();
}

void DxCore::RenderEngineUI(){
#ifdef _DEBUG


	//ImGui::End();
#endif // _DEBUG

}