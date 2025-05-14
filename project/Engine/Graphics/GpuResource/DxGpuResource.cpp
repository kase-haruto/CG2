#include "DxGpuResource.h"
/* ========================================================================
/*	include space
/* ===================================================================== */
// engine
#include <Engine/Graphics/Descriptor/SrvLocator.h>

// c++
#include <stdexcept>

void DxGpuResource::InitializeAsRenderTarget(ID3D12Device* device,
											 uint32_t width,
											 uint32_t height,
											 DXGI_FORMAT format,
											 std::optional<std::wstring> name){
	D3D12_RESOURCE_DESC texDesc = {};
	texDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	texDesc.Width = width;
	texDesc.Height = height;
	texDesc.DepthOrArraySize = 1;
	texDesc.MipLevels = 1;
	texDesc.Format = format;
	texDesc.SampleDesc.Count = 1;
	texDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	texDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

	D3D12_HEAP_PROPERTIES heapProps = {};
	heapProps.Type = D3D12_HEAP_TYPE_DEFAULT;

	D3D12_CLEAR_VALUE clearValue = {};
	clearValue.Format = format;
	clearValue.Color[0] = 0.1f;
	clearValue.Color[1] = 0.1f;
	clearValue.Color[2] = 0.1f;
	clearValue.Color[3] = 1.0f;

	HRESULT hr = device->CreateCommittedResource(
		&heapProps,
		D3D12_HEAP_FLAG_NONE,
		&texDesc,
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		&clearValue,
		IID_PPV_ARGS(&resource_)
	);

	if (FAILED(hr)){
		throw std::runtime_error("Failed to create render target resource.");
	}

	if (name.has_value()){
		resource_->SetName(name->c_str());
	}
}

void DxGpuResource::SetCurrentState(D3D12_RESOURCE_STATES state){
	currentState_ = state;
}

D3D12_RESOURCE_STATES DxGpuResource::GetCurrentState() const{
	return currentState_;
}

void DxGpuResource::Transition(ID3D12GraphicsCommandList* cmdList, D3D12_RESOURCE_STATES newState){
	if (!resource_ || currentState_ == newState) return;

	D3D12_RESOURCE_BARRIER barrier {};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Transition.pResource = resource_.Get();
	barrier.Transition.StateBefore = currentState_;
	barrier.Transition.StateAfter = newState;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

	cmdList->ResourceBarrier(1, &barrier);
	currentState_ = newState;
}

void DxGpuResource::CreateSRV(ID3D12Device* device){
	auto [cpuHandle, gpuHandle] = SrvLocator::AllocateSrv();
	cpuSrvHandle_ = cpuHandle;
	gpuSrvHandle_ = gpuHandle;

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = resource_->GetDesc().Format;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;

	device->CreateShaderResourceView(resource_.Get(), &srvDesc, cpuHandle);
}

void DxGpuResource::CreateRTV(ID3D12Device* device, D3D12_CPU_DESCRIPTOR_HANDLE handle){
	cpuRtvHandle_ = handle;

	device->CreateRenderTargetView(resource_.Get(), nullptr, handle);
}
