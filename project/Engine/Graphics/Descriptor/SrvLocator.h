#pragma once

/* c++ */
#include <wrl.h>
#include <d3d12.h>
#include <cstdint>
#include <mutex>
#include <utility>
#include <stack>

using Microsoft::WRL::ComPtr;

class SrvLocator {
public:
	// サービスを提供するための初期化メソッド
	static void Provide(const ComPtr<ID3D12DescriptorHeap>& srvHeap, const ComPtr<ID3D12Device>& device);

	// SRVを割り当てるメソッド
	static std::pair<D3D12_CPU_DESCRIPTOR_HANDLE, D3D12_GPU_DESCRIPTOR_HANDLE> AllocateSrv();

	// SRVヒープを取得するメソッド
	static ComPtr<ID3D12DescriptorHeap> GetSrvHeap();

	static void Finalize();
	static uint32_t GetCurrentOffset();
	static void FreeSrv(D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle);
private:
	static void CheckHeapAvailable(uint32_t requested);


private:
	static ComPtr<ID3D12DescriptorHeap> srvHeap_;
	static uint32_t descriptorSizeSrv_;
	static uint32_t currentOffset_;
	static std::mutex mutex_; // スレッドセーフのためのミューテックス
private:
	static std::stack<uint32_t> freeList_;  // 解放済みSRVのオフセット
	static const uint32_t kMaxSrvNum = 60000; // SRVヒープの最大スロット数
};