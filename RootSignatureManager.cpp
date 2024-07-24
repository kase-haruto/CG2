#include "RootSignatureManager.h"
#include<cassert>

bool RootSignatureManager::CreateRootSignatrue(const std::string& name, const D3D12_ROOT_SIGNATURE_DESC& rootSignatureDesc){
	ComPtr<ID3DBlob>signature;
	ComPtr<ID3DBlob>error;
	HRESULT hr = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error);

	if (FAILED(hr)){
		return false;
	}

	ComPtr<ID3D12RootSignature> rootSignature;
	hr = device_->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&rootSignature));
	if (FAILED(hr)){
		return false;
	}

	rootSignatures_[name] = rootSignature;
	return true;
}

const ComPtr<ID3D12RootSignature>& RootSignatureManager::GetRootSignature(const std::string& name) const{
	auto it = rootSignatures_.find(name);
	assert(it != rootSignatures_.end());
	return it->second;
}