// DxCore.cpp
#include "DxCore.h"

#include "Engine/core/DirectX/RenderTarget/OffscreenRenderTarget.h"
#include "Engine/core/DirectX/RenderTarget/SwapChainRenderTarget.h"
#include "Engine/graphics/GraphicsGroup.h"
#include "engine/core/Enviroment.h"
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

	SetViewPortAndScissor(width, height);

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

	// RTV heap（SwapChain用に2つ、Offscreen用に1つ = 合計3つ）
	D3D12_DESCRIPTOR_HEAP_DESC rtvDesc = {};
	rtvDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvDesc.NumDescriptors = 5;
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
	renderTargetCollection_->Add("BackBuffer", std::move(swapchainRT));

	// Offscreen はスロット2を使用
	D3D12_CPU_DESCRIPTOR_HANDLE offscreenRTVHandle = baseRTVHandle;
	offscreenRTVHandle.ptr += rtvDescriptorSize_ * 2;

	auto offscreenRT = std::make_unique<OffscreenRenderTarget>();
	offscreenRT->Initialize(device.Get(), width, height, format_, offscreenRTVHandle, dsvHandle);
	renderTargetCollection_->Add("Offscreen", std::move(offscreenRT));

	D3D12_CPU_DESCRIPTOR_HANDLE postEffectRTVHandle = baseRTVHandle;
	postEffectRTVHandle.ptr += rtvDescriptorSize_ * 3;
	auto postEffectRT = std::make_unique<OffscreenRenderTarget>();
	postEffectRT->Initialize(device.Get(), width, height, format_, /*RTV*/ postEffectRTVHandle, /*DSV*/ dsvHandle);
	renderTargetCollection_->Add("PostEffectOutput", std::move(postEffectRT));


	// debug
	D3D12_CPU_DESCRIPTOR_HANDLE debugRTVHandle = baseRTVHandle;
	debugRTVHandle.ptr += rtvDescriptorSize_ * 4;

	auto debugRT = std::make_unique<OffscreenRenderTarget>();
	debugRT->Initialize(device.Get(), width, height, format_, debugRTVHandle, dsvHandle);

	renderTargetCollection_->Add("DebugView", std::move(debugRT));
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

	commandList->RSSetViewports(1, &viewport_);
	commandList->RSSetScissorRects(1, &scissorRect_);
}

void DxCore::PreDrawOffscreen(){
	auto* offscreenTarget = renderTargetCollection_->Get("Offscreen");
	if (offscreenTarget){
		offscreenTarget->Clear(dxCommand_->GetCommandList().Get());
		offscreenTarget->SetRenderTarget(dxCommand_->GetCommandList().Get());

		dxCommand_->GetCommandList()->RSSetViewports(1, &viewport_);
		dxCommand_->GetCommandList()->RSSetScissorRects(1, &scissorRect_);
	}
}

void DxCore::DrawOffscreenTexture(){
	auto commandList = dxCommand_->GetCommandList();

	// SwapChainRenderTarget の設定
	auto* backBufferTarget = renderTargetCollection_->Get("BackBuffer");
	if (auto* swapchainRT = dynamic_cast< SwapChainRenderTarget* >(backBufferTarget)){
		UINT bufferIndex = dxSwapChain_->GetCurrentBackBufferIndex();
		swapchainRT->SetBufferIndex(bufferIndex);
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


void DxCore::SetViewPortAndScissor(uint32_t width, uint32_t height){
	viewport_ = {0.0f, 0.0f, static_cast< FLOAT >(width), static_cast< FLOAT >(height), 0.0f, 1.0f};
	scissorRect_ = {0, 0, static_cast< LONG >(width), static_cast< LONG >(height)};
}



void DxCore::RenderEngineUI(){
#ifdef _DEBUG


	//ImGui::End();
#endif // _DEBUG

}