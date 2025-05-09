#pragma once

#include "engine/core/WinApp.h"
#include "engine/core/DirectX/DxDevice.h"
#include "engine/core/DirectX/DxCommand.h"
#include "engine/core/DirectX/DxFence.h"
#include "engine/core/DirectX/DxSwapChain.h"
#include "Engine/core/DirectX/RenderTarget/RenderTargetCollection.h"
#include "engine/core/DirectX/ResourceStateTracker/ResourceStateTracker.h"
#include <memory>

using Microsoft::WRL::ComPtr;

class DxCore{
public:
	DxCore() = default;
	~DxCore();

	// 初期化
	void Initialize(WinApp* winApp, uint32_t width, uint32_t height);

	// オフスクリーンレンダラー初期化
	void RendererInitialize(uint32_t width, uint32_t height);

	// 描画処理
	void PreDraw();
	void PreDrawOffscreen();
	void DrawOffscreenTexture();
	void PostDraw();
	void RenderEngineUI();

private:

	// リソース解放
	void ReleaseResources();

public:
	const ComPtr<ID3D12Device>& GetDevice() const{ return dxDevice_->GetDevice(); }
	const ComPtr<ID3D12GraphicsCommandList>& GetCommandList() const{ return dxCommand_->GetCommandList(); }
	const DxSwapChain& GetSwapChain() const{ return *dxSwapChain_; }
	const RenderTargetCollection& GetRenderTargetCollection() const{ return *renderTargetCollection_; }
	// メソッド追加
	DXGI_FORMAT GetFormat() const{ return format_; }
private:
	WinApp* winApp_ = nullptr;
	uint32_t clientWidth_ = 0;
	uint32_t clientHeight_ = 0;


	// DirectX関連
	std::unique_ptr<DxDevice> dxDevice_;
	std::unique_ptr<DxCommand> dxCommand_;
	std::unique_ptr<DxSwapChain> dxSwapChain_;
	
	ComPtr<ID3D12DescriptorHeap> rtvHeap_;
	ComPtr<ID3D12DescriptorHeap> dsvHeap_;
	UINT rtvDescriptorSize_ = 0;
	DXGI_FORMAT format_ = DXGI_FORMAT_R8G8B8A8_UNORM; // 仮の初期値
	std::unique_ptr<RenderTargetCollection> renderTargetCollection_;
	std::unique_ptr<DxFence> dxFence_;

};
