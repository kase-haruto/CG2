#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <array>
#include <cassert>

#include "Vector4.h"

#include"core/DirectX/DxSwapChain.h"

class RenderTarget{
    template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public:
    /// <summary>
    /// 初期化
    /// </summary>
    /// <param name="device"></param>
    /// <param name="swapChain"></param>
    void Initialize(ComPtr<ID3D12Device> device, DxSwapChain& swapChain);

    /// <summary>
    /// オフスクリーンレンダーターゲットの初期化
    /// </summary>
    /// <param name="device"></param>
    /// <param name="width"></param>
    /// <param name="height"></param>
    void InitializeOffscreen(ComPtr<ID3D12Device> device, uint32_t width, uint32_t height,const Vector4& color);

    /// <summary>
    /// 深度バッファの生成
    /// </summary>
    /// <param name="device"></param>
    /// <param name="width"></param>
    /// <param name="height"></param>
    void CreateDepthBuffer(ComPtr<ID3D12Device> device, uint32_t width, uint32_t height);

    /// <summary>
    /// レンダーターゲットのクリア
    /// </summary>
    /// <param name="commandList"></param>
    /// <param name="backBufferIndex"></param>
    void ClearRenderTarget(ComPtr<ID3D12GraphicsCommandList> commandList, UINT backBufferIndex);

    /// <summary>
    /// 深度ステンシルテクスチャリソースの生成
    /// </summary>
    /// <param name="device"></param>
    /// <param name="width"></param>
    /// <param name="height"></param>
    ComPtr<ID3D12Resource> CreateDepthStencilTextureResource(ID3D12Device* device, int32_t width, int32_t height);

public:
    ///////////////////////////////////////////////////
    //              リソース
    ///////////////////////////////////////////////////
    D3D12_RENDER_TARGET_VIEW_DESC rtvDesc_ {};
    D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc_ = {};
    ComPtr<ID3D12DescriptorHeap> rtvHeap_ = nullptr;
    ComPtr<ID3D12DescriptorHeap> dsvHeap_ = nullptr;
    ComPtr<ID3D12Resource> depthStencilResource_ = nullptr;
    ComPtr<ID3D12Resource> offscreenRenderTarget_ = nullptr; // オフスクリーンレンダーターゲット
    std::array<ComPtr<ID3D12Resource>, 2> backBuffers_; // バックバッファの配列
    UINT rtvDescriptorSize_ = 0; // RTVディスクリプタのサイズ

    /// <summary>
    /// オフスクリーンレンダーターゲットのRTVハンドルを取得
    /// </summary>
    /// <returns></returns>
    D3D12_CPU_DESCRIPTOR_HANDLE GetOffscreenRTVHandle() const{
        return rtvHeap_->GetCPUDescriptorHandleForHeapStart();
    }

    /// <summary>
    /// オフスクリーンレンダーターゲットを取得
    /// </summary>
    /// <returns></returns>
    ComPtr<ID3D12Resource> GetOffscreenRenderTarget() const{
        return offscreenRenderTarget_;
    }

    /// <summary>
    /// スワップチェーンのRTVハンドルを取得
    /// </summary>
    /// <param name="index"></param>
    /// <returns></returns>
    D3D12_CPU_DESCRIPTOR_HANDLE GetSwapChainRTVHandle(UINT index) const{
        D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = rtvHeap_->GetCPUDescriptorHandleForHeapStart();
        rtvHandle.ptr += index * rtvDescriptorSize_;
        return rtvHandle;
    }

    D3D12_CPU_DESCRIPTOR_HANDLE GetDSVHandle() const{
        return dsvHeap_->GetCPUDescriptorHandleForHeapStart();
    }
};