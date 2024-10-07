#include "DxFunc.h"

ComPtr<ID3D12DescriptorHeap> CreateDescriptorHeap(
	ComPtr<ID3D12Device> device,
	D3D12_DESCRIPTOR_HEAP_TYPE heapType,
	UINT numDescriptors,
	bool shaderVisible){

	ComPtr<ID3D12DescriptorHeap> descriptorHeap = nullptr;
	D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc {};
	descriptorHeapDesc.Type = heapType;
	descriptorHeapDesc.NumDescriptors = numDescriptors;
	descriptorHeapDesc.Flags = shaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	HRESULT hr = device->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&descriptorHeap));
	assert(SUCCEEDED(hr));
	return descriptorHeap;
}