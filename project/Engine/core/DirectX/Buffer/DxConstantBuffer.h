#pragma once

/////////////////////////////////////////////////////////////////////////////////////////
//	include
/////////////////////////////////////////////////////////////////////////////////////////

#include "DxBuffer.h"

template<typename T>
class DxConstantBuffer 
	: public DxBuffer<T>{
public:
	//===================================================================*/
	//                   public functions
	//===================================================================*/
	void Initialize(ComPtr<ID3D12Device> device, UINT elementCount, const T* data = nullptr) override{
		this->elementCount_ = elementCount;
		size_t byteSize = ((sizeof(T) * elementCount + 255) & ~255); // 256バイトアライメント
		this->CreateUploadResource(device, byteSize);
		if (data){
			this->TransferData(data, elementCount);
		}
	}

	void SetCommand(ComPtr<ID3D12GraphicsCommandList> cmdList, UINT rootParameterIndex) override{
		cmdList->SetGraphicsRootConstantBufferView(rootParameterIndex, this->resource_->GetGPUVirtualAddress());
	}
};
