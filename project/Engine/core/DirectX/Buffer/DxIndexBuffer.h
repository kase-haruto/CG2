#pragma once

/////////////////////////////////////////////////////////////////////////////////////////
//	include
/////////////////////////////////////////////////////////////////////////////////////////

#include "DxBuffer.h"

template<typename T>
class DxIndexBuffer 
	: public DxBuffer<T>{
public:
	//===================================================================*/
	//                   public functions
	//===================================================================*/
	void Initialize(ComPtr<ID3D12Device> device, UINT elementCount, const T* data = nullptr) override{
		this->elementCount_ = elementCount;
		size_t byteSize = sizeof(T) * elementCount;
		this->CreateUploadResource(device, byteSize);

		indexBufferView_.BufferLocation = this->resource_->GetGPUVirtualAddress();
		indexBufferView_.SizeInBytes = static_cast< UINT >(byteSize);
		indexBufferView_.Format = (std::is_same_v<T, uint16_t>) ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;

		if (data){
			this->TransferData(data, elementCount);
		}
	}

	void SetCommand(ComPtr<ID3D12GraphicsCommandList> cmdList,[[maybe_unused]] UINT rootParameterIndex =0) override{
		cmdList->IASetIndexBuffer(&indexBufferView_);
	}

	// viewの取得 ===================================================================*/
	const D3D12_INDEX_BUFFER_VIEW& GetIndexBufferView() const{ return indexBufferView_; }

private:
	D3D12_INDEX_BUFFER_VIEW indexBufferView_ = {};
};
