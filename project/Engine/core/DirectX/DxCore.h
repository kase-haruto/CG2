#pragma once

#include "engine/core/WinApp.h"
#include "engine/core/DirectX/DxDevice.h"
#include "engine/core/DirectX/DxCommand.h"
#include "engine/core/DirectX/DxFence.h"
#include "engine/core/DirectX/DxSwapChain.h"
#include "engine/core/DirectX/RenderTarget.h"
#include "engine/core/DirectX/ResourceStateTracker.h"
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
    void SetViewPortAndScissor(uint32_t width, uint32_t height);

    void TransitionResource(ID3D12GraphicsCommandList* commandList, ID3D12Resource* resource, D3D12_RESOURCE_STATES afterState);


    // リソース解放
    void ReleaseResources();

public:
    const ComPtr<ID3D12Device>& GetDevice() const{ return dxDevice_->GetDevice(); }
    const ComPtr<ID3D12GraphicsCommandList>& GetCommandList() const{ return dxCommand_->GetCommandList(); }
    const DxSwapChain& GetSwapChain() const{ return *dxSwapChain_; }
    const RenderTarget& GetRenderTarget() const{ return *renderTarget_; }

private:
    WinApp* winApp_ = nullptr;
    uint32_t clientWidth_ = 0;
    uint32_t clientHeight_ = 0;

    // ビューポートとシザー矩形
    D3D12_VIEWPORT viewport_ {};
    D3D12_RECT scissorRect_ {};

    // DirectX関連
    std::unique_ptr<DxDevice> dxDevice_;
    std::unique_ptr<DxCommand> dxCommand_;
    std::unique_ptr<DxSwapChain> dxSwapChain_;
    std::unique_ptr<RenderTarget> renderTarget_;
    std::unique_ptr<DxFence> dxFence_;

    ResourceStateTracker resourceStateTracker_;
};
