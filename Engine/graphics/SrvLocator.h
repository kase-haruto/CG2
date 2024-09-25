#pragma once
#include <wrl.h>
#include <d3d12.h>
#include <cstdint>
#include <mutex>
#include <utility>  // std::move を使用するために必要

using Microsoft::WRL::ComPtr;

class SrvLocator{
public:
    // サービスを提供するための初期化メソッド
    static void Provide(ComPtr<ID3D12DescriptorHeap> srvHeap, ComPtr<ID3D12Device> device);

    // SRVを割り当てるメソッド
    static std::pair<D3D12_CPU_DESCRIPTOR_HANDLE, D3D12_GPU_DESCRIPTOR_HANDLE> AllocateSrv();

    // SRVヒープを取得するメソッド
    static ComPtr<ID3D12DescriptorHeap> GetSrvHeap();

private:
    static ComPtr<ID3D12DescriptorHeap> srvHeap_;
    static uint32_t descriptorSizeSrv_;
    static uint32_t currentOffset_;
    static std::mutex mutex_; // スレッドセーフのためのミューテックス
};