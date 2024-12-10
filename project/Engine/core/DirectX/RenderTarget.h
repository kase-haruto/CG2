#pragma once
#include "engine/core/DirectX/DxSwapChain.h"

/* c++ */
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <array>
#include <cassert>


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

    ComPtr<ID3D12Resource> CreateDepthStencilTextureResource(ID3D12Device* device, int32_t width, int32_t height);


    // オフスクリーンターゲット作成
    void CreateOffscreenRenderTarget(ComPtr<ID3D12Device> device, uint32_t width, uint32_t height);
public:
    ///////////////////////////////////////////////////
    //              リソース
    ///////////////////////////////////////////////////
    D3D12_RENDER_TARGET_VIEW_DESC rtvDesc_ {};
    ComPtr<ID3D12DescriptorHeap> rtvHeap_ = nullptr;
    ComPtr<ID3D12DescriptorHeap> dsvHeap_ = nullptr;
    ComPtr<ID3D12Resource> depthStencilResource_ = nullptr;
    std::array<ComPtr<ID3D12Resource>, 2> backBuffers_; // バックバッファの配列
    UINT rtvDescriptorSize_ = 0; // RTVディスクリプタのサイズ

    // オフスクリーン用メンバ
    ComPtr<ID3D12Resource> offscreenRenderTarget_ = nullptr;
    ComPtr<ID3D12DescriptorHeap> offscreenRtvHeap_ = nullptr;
    UINT offscreenRtvDescriptorSize_ = 0;

    D3D12_RESOURCE_STATES offscreenRenderTargetState_ = D3D12_RESOURCE_STATE_COMMON;
    D3D12_RESOURCE_STATES backBufferStates_[2] = {D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_PRESENT};
};
