#pragma once
#include "IRenderTarget.h"
#include "engine/core/DirectX/GpuResource/DxGpuResource.h"

#include <memory>

class OffscreenRenderTarget
	: public IRenderTarget{
public:
	void Initialize(ID3D12Device* device, uint32_t width, uint32_t height, DXGI_FORMAT format,
					D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle,
					D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle);

	ID3D12Resource* GetResource() const override;
	D3D12_CPU_DESCRIPTOR_HANDLE GetRTV() const override;
	D3D12_CPU_DESCRIPTOR_HANDLE GetDSV() const override;
	D3D12_GPU_DESCRIPTOR_HANDLE GetSRV() const override;
	D3D12_VIEWPORT GetViewport() const override;
	D3D12_RECT GetScissorRect() const override;
	void Clear(ID3D12GraphicsCommandList* cmdList) override;
	void SetRenderTarget(ID3D12GraphicsCommandList* commandList)override;

	void TransitionTo(ID3D12GraphicsCommandList* cmdList, D3D12_RESOURCE_STATES newState);

private:
	DxGpuResource resource_;
	Microsoft::WRL::ComPtr<ID3D12Resource> depthBuffer_;
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle_ {};
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle_ {};

	D3D12_VIEWPORT viewport_ {};
	D3D12_RECT scissorRect_ {};
};
