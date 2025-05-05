#pragma once
#include "IRenderTarget.h"
#include "engine/core/DirectX/DxSwapChain.h"

class SwapChainRenderTarget : public IRenderTarget{
public:
	void Initialize(DxSwapChain* swapChain, ID3D12DescriptorHeap* rtvHeap, UINT rtvDescriptorSize);
	void SetBufferIndex(UINT index);

	ID3D12Resource* GetResource() const override;
	D3D12_CPU_DESCRIPTOR_HANDLE GetRTV() const override;
	D3D12_CPU_DESCRIPTOR_HANDLE GetDSV() const override{ return {}; }
	D3D12_GPU_DESCRIPTOR_HANDLE GetSRV() const override{ return {}; }
	void SetRenderTarget(ID3D12GraphicsCommandList* commandList) override;
	void TransitionTo(ID3D12GraphicsCommandList* commandList, D3D12_RESOURCE_STATES newState) override;
	void Clear(ID3D12GraphicsCommandList* cmdList) override;

private:
	DxSwapChain* swapChain_ = nullptr;
	ID3D12DescriptorHeap* rtvHeap_ = nullptr;
	UINT rtvDescriptorSize_ = 0;
	UINT bufferIndex_ = 0;

	std::vector<D3D12_RESOURCE_STATES> currentStates_;
};