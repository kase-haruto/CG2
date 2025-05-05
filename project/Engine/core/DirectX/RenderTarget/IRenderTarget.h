#pragma once
#include <d3d12.h>

class IRenderTarget{
public:
	virtual ~IRenderTarget() = default;
	virtual ID3D12Resource* GetResource() const = 0;
	virtual D3D12_CPU_DESCRIPTOR_HANDLE GetRTV() const = 0;
	virtual D3D12_CPU_DESCRIPTOR_HANDLE GetDSV() const = 0;
	virtual D3D12_GPU_DESCRIPTOR_HANDLE GetSRV() const = 0;
	virtual D3D12_VIEWPORT GetViewport() const = 0;
	virtual D3D12_RECT GetScissorRect() const = 0;
	virtual void SetRenderTarget(ID3D12GraphicsCommandList* commandList) = 0;
	virtual void TransitionTo([[maybe_unused]] ID3D12GraphicsCommandList* cmdList,[[maybe_unused]] D3D12_RESOURCE_STATES newState){};
	virtual void Clear(ID3D12GraphicsCommandList* cmdList) = 0;
};
