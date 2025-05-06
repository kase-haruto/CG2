#include "SwapChainRenderTarget.h"
#include <Engine/core/Enviroment.h>
#include <cassert>

void SwapChainRenderTarget::Initialize(DxSwapChain* swapChain, ID3D12DescriptorHeap* rtvHeap, UINT rtvDescriptorSize){
	swapChain_ = swapChain;
	rtvHeap_ = rtvHeap;
	rtvDescriptorSize_ = rtvDescriptorSize;

	UINT backBufferCount = 2;
	currentStates_.resize(backBufferCount, D3D12_RESOURCE_STATE_PRESENT);

	viewport_ = {0.0f, 0.0f,kWindowWidth , kWindowHeight, 0.0f, 1.0f};
	scissorRect_ = {0, 0, kWindowWidth, kWindowHeight};
}

void SwapChainRenderTarget::SetBufferIndex(UINT index){
	bufferIndex_ = index;
}

D3D12_CPU_DESCRIPTOR_HANDLE SwapChainRenderTarget::GetRTV() const{
	D3D12_CPU_DESCRIPTOR_HANDLE handle = rtvHeap_->GetCPUDescriptorHandleForHeapStart();
	handle.ptr += rtvDescriptorSize_ * bufferIndex_;
	return handle;
}

void SwapChainRenderTarget::SetRenderTarget(ID3D12GraphicsCommandList* commandList){
	TransitionTo(commandList, D3D12_RESOURCE_STATE_RENDER_TARGET);
	D3D12_CPU_DESCRIPTOR_HANDLE handle = GetRTV();
	commandList->OMSetRenderTargets(1, &handle, FALSE, nullptr);
}


void SwapChainRenderTarget::Clear(ID3D12GraphicsCommandList* cmdList){
	TransitionTo(cmdList, D3D12_RESOURCE_STATE_RENDER_TARGET);
	float clearColor[] = {1.02f, 0.02f, 0.02f, 1.0f};
	cmdList->ClearRenderTargetView(GetRTV(), clearColor, 0, nullptr);
}

void SwapChainRenderTarget::TransitionTo(ID3D12GraphicsCommandList* commandList, D3D12_RESOURCE_STATES newState){
	if (bufferIndex_ >= currentStates_.size()) return; // 安全ガード

	D3D12_RESOURCE_STATES& currentState = currentStates_[bufferIndex_];
	if (currentState == newState) return;

	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = swapChain_->GetBackBuffer(bufferIndex_).Get();
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Transition.StateBefore = currentState;
	barrier.Transition.StateAfter = newState;
	commandList->ResourceBarrier(1, &barrier);

	currentState = newState;
}