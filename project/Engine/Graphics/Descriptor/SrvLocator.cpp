#include "SrvLocator.h"

Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> SrvLocator::srvHeap_ = nullptr;
uint32_t SrvLocator::descriptorSizeSrv_ = 0;
uint32_t SrvLocator::currentOffset_ = 0;
std::mutex SrvLocator::mutex_;

void SrvLocator::Provide(const ComPtr<ID3D12DescriptorHeap>& srvHeap, const ComPtr<ID3D12Device>& device) {
	std::lock_guard<std::mutex> lock(mutex_);

	if (srvHeap_ != nullptr) {
		// 既に提供された場合、再設定を防ぐ
		throw std::runtime_error("SrvLocator already initialized.");
	}

	srvHeap_ = srvHeap;
	descriptorSizeSrv_ = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	currentOffset_ = 1; // ImGui用の最初のスロットを予約
}

std::pair<D3D12_CPU_DESCRIPTOR_HANDLE, D3D12_GPU_DESCRIPTOR_HANDLE> SrvLocator::AllocateSrv() {
	std::lock_guard<std::mutex> lock(mutex_);

	if (!srvHeap_) {
		throw std::runtime_error("SrvLocator has not been provided with a descriptor heap.");
	}

	D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = srvHeap_->GetCPUDescriptorHandleForHeapStart();
	D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle = srvHeap_->GetGPUDescriptorHandleForHeapStart();
	cpuHandle.ptr += currentOffset_ * descriptorSizeSrv_;
	gpuHandle.ptr += currentOffset_ * descriptorSizeSrv_;
	currentOffset_++;
	return { cpuHandle, gpuHandle };
}

ComPtr<ID3D12DescriptorHeap> SrvLocator::GetSrvHeap() {
	std::lock_guard<std::mutex> lock(mutex_);
	return srvHeap_;
}

void SrvLocator::Finalize() {
	srvHeap_.Reset();
}

uint32_t SrvLocator::GetCurrentOffset() {
	std::lock_guard<std::mutex> lock(mutex_);
	return currentOffset_;
}


void SrvLocator::CheckHeapAvailable(uint32_t requested) {
	if (currentOffset_ + requested > kMaxSrvNum) {
		throw std::runtime_error("Not enough descriptors available in the heap.");
	}
}