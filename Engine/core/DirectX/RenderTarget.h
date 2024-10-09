#pragma once
#include "Vector4.h"
#include "core/DirectX/DxSwapChain.h"

class RenderTarget{
    template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public:
    // 初期化
    void Initialize(ComPtr<ID3D12Device> device, DxSwapChain& swapChain);

    // 深度バッファの生成
    void CreateDepthBuffer(ComPtr<ID3D12Device> device, uint32_t width, uint32_t height);

    // レンダーターゲットのクリア
    void ClearRenderTarget(ComPtr<ID3D12GraphicsCommandList> commandList, UINT backBufferIndex);

    // オフスクリーン用レンダーターゲットのクリア
    void ClearOffscreenRenderTarget(ComPtr<ID3D12GraphicsCommandList> commandList);

    // 深度ステンシルリソースの作成
    ComPtr<ID3D12Resource> CreateDepthStencilResource(ComPtr<ID3D12Device> device, int32_t width, int32_t height);

    // オフスクリーン用レンダーターゲットリソースの作成
    ComPtr<ID3D12Resource> CreateRenderTextureResource(
        ComPtr<ID3D12Device> device,
        uint32_t width, uint32_t height,
        DXGI_FORMAT format,
        const Vector4& clearColor);

    // オフスクリーン用RTVの作成
    void CreateOffscreenRTV(ComPtr<ID3D12Device> device);

    // オフスクリーン用SRVの作成
    void CreateOffscreenSRV(ComPtr<ID3D12Device> device, ComPtr<ID3D12DescriptorHeap> srvHeap);

public:
    // リソース
    D3D12_RENDER_TARGET_VIEW_DESC rtvDesc_ {};
    ComPtr<ID3D12DescriptorHeap> rtvHeap_ = nullptr;
    ComPtr<ID3D12DescriptorHeap> dsvHeap_ = nullptr;
    ComPtr<ID3D12Resource> depthStencilResource_ = nullptr;
    ComPtr<ID3D12Resource> renderTextureResource_ = nullptr;
    std::array<ComPtr<ID3D12Resource>, 2> backBuffers_; // バックバッファの配列
    UINT rtvDescriptorSize_ = 0; // RTVディスクリプタのサイズ
};
