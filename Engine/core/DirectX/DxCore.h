#pragma once

#include "WinApp.h"

/*-- dx関連 --*/
#include "core/DirectX/DxDevice.h"
#include "core/DirectX/DxCommand.h"
#include "core/DirectX/DxFence.h"
#include "core/DirectX/DxSwapChain.h"
#include "core/DirectX/RenderTarget.h"

#include <memory>

using Microsoft::WRL::ComPtr;

class DxCore{
public:
    DxCore() = default;
    ~DxCore();

    /// <summary>
    /// 初期化
    /// </summary>
    /// <param name="winApp"></param>
    /// <param name="width"></param>
    /// <param name="height"></param>
    void Initialize(WinApp* winApp, uint32_t width, uint32_t height);

    /// <summary>
    /// 描画前処理
    /// </summary>
    void PreDraw();

    /// <summary>
    /// 描画後処理
    /// </summary>
    void PostDraw();

private:
    /// <summary>
    /// ビューポートとシザーレクとの設定
    /// </summary>
    /// <param name="width"></param>
    /// <param name="height"></param>
    void SetViewPortAndScissor(uint32_t width, uint32_t height);


    /////////////////////////////////////////////////////////////////////////////////////////
    /*                                           アクセッサ                                  */
    /////////////////////////////////////////////////////////////////////////////////////////
public:
    const ComPtr<ID3D12Device>& GetDevice() const{ return dxDevice_->GetDevice(); }
    const ComPtr<ID3D12GraphicsCommandList>& GetCommandList() const{ return dxCommand_->GetCommandList(); }
    const DxSwapChain& GetSwapChain() const{ return *dxSwapChain_; }
    const RenderTarget& GetRenderTarget() const{ return *renderTarget_; }

private:
    WinApp* winApp_ = nullptr;
    uint32_t clientWidth_ = 0;
    uint32_t clientHeight_ = 0;

    // ビューポート
    D3D12_VIEWPORT viewport_ {};
    // シザー矩形
    D3D12_RECT scissorRect_ {};

    // 各種DirectX関連
    std::unique_ptr<DxDevice> dxDevice_;
    std::unique_ptr<DxCommand> dxCommand_;
    std::unique_ptr<DxSwapChain> dxSwapChain_;
    std::unique_ptr<RenderTarget> renderTarget_;
    std::unique_ptr<DxFence> dxFence_;
};
