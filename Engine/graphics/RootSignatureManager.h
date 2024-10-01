#pragma once

#include<d3d12.h>
#include<wrl.h>
#include<unordered_map>
#include<string>

using Microsoft::WRL::ComPtr;

class RootSignatureManager{
public:
	RootSignatureManager(ID3D12Device* device) :device_(device){}
	~RootSignatureManager(){ device_.Reset(); }

	bool CreateRootSignatrue(const std::string& name, const D3D12_ROOT_SIGNATURE_DESC& rootSignatureDesc);

	const ComPtr<ID3D12RootSignature>&  GetRootSignature(const std::string& name) const;
private:
	ComPtr<ID3D12Device> device_;
	std::unordered_map<std::string, ComPtr<ID3D12RootSignature>>rootSignatures_;
};

