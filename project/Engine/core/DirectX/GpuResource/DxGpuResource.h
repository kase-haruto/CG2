#pragma once

#include <wrl.h>
#include <d3d12.h>
#include <string>
#include <optional>

using Microsoft::WRL::ComPtr;

class DxGpuResource{
public:
	DxGpuResource() = default;
	~DxGpuResource() = default;

	// リソースの生成
	void InitializeAsRenderTarget(ID3D12Device* device,
								  uint32_t width,
								  uint32_t height,
								  DXGI_FORMAT format,
								  std::optional<std::wstring> name = std::nullopt);

	// SRV/RTV作成（SrvLocatorを使用）
	void CreateSRV(ID3D12Device* device);
	void CreateRTV(ID3D12Device* device, D3D12_CPU_DESCRIPTOR_HANDLE handle);

	// アクセサ
	ID3D12Resource* Get() const{ return resource_.Get(); }
	D3D12_CPU_DESCRIPTOR_HANDLE GetSRVCpuHandle() const{ return cpuSrvHandle_; }
	D3D12_GPU_DESCRIPTOR_HANDLE GetSRVGpuHandle() const{ return gpuSrvHandle_; }
	D3D12_CPU_DESCRIPTOR_HANDLE GetRTVCpuHandle() const{ return cpuRtvHandle_; }

private:
	ComPtr<ID3D12Resource> resource_;
	D3D12_CPU_DESCRIPTOR_HANDLE cpuSrvHandle_ {};
	D3D12_GPU_DESCRIPTOR_HANDLE gpuSrvHandle_ {};
	D3D12_CPU_DESCRIPTOR_HANDLE cpuRtvHandle_ {};
};
