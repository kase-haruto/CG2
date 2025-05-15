#pragma once

/////////////////////////////////////////////////////////////////////////////////////////
//	include
/////////////////////////////////////////////////////////////////////////////////////////

#include "DxBuffer.h"

template<typename T>
class DxVertexBuffer
	: public DxBuffer<T>{
public:
	//===================================================================*/
	//                   public functions
	//===================================================================*/
	void Initialize(ComPtr<ID3D12Device> device, UINT elementCount = 1) override;

	void SetCommand(ComPtr<ID3D12GraphicsCommandList> cmdList, [[maybe_unused]] UINT rootParameterIndex = 0)const override{
		if (!this->resource_){
			assert(false && "VertexBuffer resource is null. Initialize() might not have been called.");
			return;
		}
			
		// **入力アセンブラに頂点バッファを設定**
		cmdList->IASetVertexBuffers(0, 1, &vertexBufferView_);
	}


	// viewの取得 ===================================================================*/
	D3D12_VERTEX_BUFFER_VIEW& GetVertexBufferView(){ return vertexBufferView_; }

private:
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_ = {};
};

template<typename T>
inline void DxVertexBuffer<T>::Initialize(ComPtr<ID3D12Device> device, UINT elementCount){
	this->elementCount_ = elementCount;
	size_t byteSize = sizeof(T) * elementCount;
	this->CreateUploadResource(device, byteSize);

	vertexBufferView_.BufferLocation = this->resource_->GetGPUVirtualAddress();
	vertexBufferView_.SizeInBytes = static_cast< UINT >(byteSize);
	vertexBufferView_.StrideInBytes = sizeof(T);
}
