#pragma once

/////////////////////////////////////////////////////////////////////////////////////////
//	include
/////////////////////////////////////////////////////////////////////////////////////////

// lib
#include <wrl.h>
#include <d3d12.h>
#include <string>
#include <vector>
#include <cassert>
#include <dxgi1_4.h>
#include <dxgiformat.h>

// using
using Microsoft::WRL::ComPtr;

/////////////////////////////////////////////////////////////////////////////////////////
//	汎用bufferクラス
/////////////////////////////////////////////////////////////////////////////////////////
template<typename T>
class DxBuffer{
public:
	//===================================================================*/
	//                   public functions
	//===================================================================*/
	DxBuffer() = default;
	virtual ~DxBuffer() = default;

	virtual void Initialize(ComPtr<ID3D12Device> device, UINT elementCount, const T* data = nullptr) = 0;
	virtual void SetCommand(ComPtr<ID3D12GraphicsCommandList>cmdList, UINT rootParameterIndex) = 0;
	virtual void TransferData(const T* data, UINT elementCount);

	// リソースの取得 ===================================================================*/
	ComPtr<ID3D12Resource> GetResource() const{ return resource_; }

protected:
	//===================================================================*/
	//                   protected functions
	//===================================================================*/
	void CreateUploadResource(ComPtr<ID3D12Device> device, size_t byteSize);

protected:
	//===================================================================*/
	//                   protected variables
	//===================================================================*/
	ComPtr<ID3D12Resource> resource_ = nullptr;
	UINT8* mappedPtr_ = nullptr;
	UINT elementCount_ = 0;
};

/////////////////////////////////////////////////////////////////////////////////////////
//	データ転送
/////////////////////////////////////////////////////////////////////////////////////////
template<typename T>
void DxBuffer<T>::TransferData(const T* data, UINT elementCount){
	assert(mappedPtr_ && "Resource is not mapped!");
	std::memcpy(mappedPtr_, data, sizeof(T) * elementCount);
}

/////////////////////////////////////////////////////////////////////////////////////////
//	bufferの作成
/////////////////////////////////////////////////////////////////////////////////////////
template<typename T>
void DxBuffer<T>::CreateUploadResource(ComPtr<ID3D12Device> device, size_t byteSize){
	D3D12_HEAP_PROPERTIES heapProps = {};
	heapProps.Type = D3D12_HEAP_TYPE_UPLOAD;

	D3D12_RESOURCE_DESC resDesc = {};
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.Width = byteSize;
	resDesc.Height = 1;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = 1;
	resDesc.Format = DXGI_FORMAT_UNKNOWN;
	resDesc.SampleDesc.Count = 1;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	HRESULT hr = device->CreateCommittedResource(
		&heapProps,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&resource_)
	);

	assert(SUCCEEDED(hr) && "Failed to create Upload Resource.");

	// マップ
	hr = resource_->Map(0, nullptr, reinterpret_cast< void** >(&mappedPtr_));
	assert(SUCCEEDED(hr) && "Failed to Map Upload Resource.");
}
