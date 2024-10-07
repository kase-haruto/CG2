#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <array>
#include <cassert>


class DxSwapChain{
    template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public:
    /// <summary>
    /// 初期化
    /// </summary>
    /// <param name="dxgiFactory"></param>
    /// <param name="commandQueue"></param>
    /// <param name="hwnd"></param>
    /// <param name="width"></param>
    /// <param name="height"></param>
    void Initialize(ComPtr<IDXGIFactory7> dxgiFactory, ComPtr<ID3D12CommandQueue> commandQueue, HWND hwnd, uint32_t width, uint32_t height);

    void Presetn();

    /////////////////////////////////////////////////////////////////////////////////////////
    //              アクセッサ
    /////////////////////////////////////////////////////////////////////////////////////////    ComPtr<ID3D12GraphicsCommandList> GetCommandList() const{ return commandList_; }
    UINT GetCurrentBackBufferIndex() const{ return swapChain_->GetCurrentBackBufferIndex(); }
    ComPtr<IDXGISwapChain4> GetSwapChain() const{ return swapChain_; }
    ComPtr<ID3D12Resource> GetBackBuffer(UINT index) const{ return backBuffers_[index]; }

private:
    ///////////////////////////////////////////////////
    //              リソース
    ///////////////////////////////////////////////////
    ComPtr<IDXGISwapChain4> swapChain_ = nullptr;
    std::array<ComPtr<ID3D12Resource>, 2> backBuffers_;
    DXGI_SWAP_CHAIN_DESC1 swapChainDesc_ {};
};
