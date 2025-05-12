#include "Texture.h"
#include <lib/myFunc/ConvertString.h>
#include <Engine/graphics/SrvLocator.h>
#include <lib/myFunc/MyFunc.h>

/* c++ */
#include <cassert>
#include <d3dx12.h>

Texture::Texture(const std::string& filePath) : filePath_(filePath) {}

Texture::~Texture() {
	// リソースの解放処理
	resource_.Reset();
}

Texture::Texture(Texture&& other) noexcept
	: filePath_(std::move(other.filePath_)),
	image_(std::move(other.image_)),
	metadata_(std::move(other.metadata_)),
	resource_(std::move(other.resource_)),
	srvHandleCPU_(other.srvHandleCPU_),
	srvHandleGPU_(other.srvHandleGPU_) {
	other.srvHandleCPU_.ptr = 0;
	other.srvHandleGPU_.ptr = 0;
}

Texture& Texture::operator=(Texture&& other) noexcept {
	if (this != &other) {
		filePath_ = std::move(other.filePath_);
		image_ = std::move(other.image_);
		metadata_ = std::move(other.metadata_);
		resource_ = std::move(other.resource_);
		srvHandleCPU_ = other.srvHandleCPU_;
		srvHandleGPU_ = other.srvHandleGPU_;

		other.srvHandleCPU_.ptr = 0;
		other.srvHandleGPU_.ptr = 0;
	}
	return *this;
}

void Texture::Load([[maybe_unused]] ID3D12Device* device) {
	std::string fullPath = "Resources/textures/" + filePath_;
	image_ = LoadTextureImage(fullPath);
	metadata_ = image_.GetMetadata();
}

void Texture::Upload(ID3D12Device* device) {
	D3D12_RESOURCE_DESC resourceDesc = {};
	resourceDesc.Width = UINT(metadata_.width);
	resourceDesc.Height = UINT(metadata_.height);
	resourceDesc.MipLevels = UINT16(metadata_.mipLevels);
	resourceDesc.DepthOrArraySize = UINT16(metadata_.arraySize);
	resourceDesc.Format = metadata_.format;
	resourceDesc.SampleDesc.Count = 1;
	resourceDesc.SampleDesc.Quality = 0;
	resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

	// ✅ Dimensionの修正（CubeMapでも2Dとして扱う）
	switch (metadata_.dimension) {
		case DirectX::TEX_DIMENSION_TEXTURE1D:
			resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE1D;
			break;
		case DirectX::TEX_DIMENSION_TEXTURE2D:
			resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
			break;
		case DirectX::TEX_DIMENSION_TEXTURE3D:
			resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE3D;
			break;
		default:
			assert(false && "Unsupported texture dimension");
	}

	D3D12_HEAP_PROPERTIES heapProperties = {};
	heapProperties.Type = D3D12_HEAP_TYPE_CUSTOM;
	heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;

	HRESULT hr = device->CreateCommittedResource(
		&heapProperties,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&resource_)
	);
	assert(SUCCEEDED(hr));

	// ✅ mip + arraySize 両方に対応した書き込み
	for (size_t item = 0; item < metadata_.arraySize; ++item) {
		for (size_t mip = 0; mip < metadata_.mipLevels; ++mip) {
			const DirectX::Image* img = image_.GetImage(mip, item, 0);
			assert(img != nullptr);

			UINT subresourceIndex = D3D12CalcSubresource(
				UINT(mip),
				UINT(item),
				0,
				UINT(metadata_.mipLevels),
				UINT(metadata_.arraySize)
			);

			hr = resource_->WriteToSubresource(
				subresourceIndex,
				nullptr,
				img->pixels,
				UINT(img->rowPitch),
				UINT(img->slicePitch)
			);
			assert(SUCCEEDED(hr));
		}
	}
}


void Texture::CreateShaderResourceView(ID3D12Device* device) {
	auto srvHandle = SrvLocator::AllocateSrv();
	srvHandleCPU_ = srvHandle.first;
	srvHandleGPU_ = srvHandle.second;

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = metadata_.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

	if (metadata_.IsCubemap()) {
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
		srvDesc.TextureCube.MostDetailedMip = 0;
		srvDesc.TextureCube.MipLevels = UINT_MAX;
		srvDesc.TextureCube.ResourceMinLODClamp = 0.0f;
	} else {
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = UINT(metadata_.mipLevels);
	}

	device->CreateShaderResourceView(resource_.Get(), &srvDesc, srvHandleCPU_);
}

const DirectX::TexMetadata& Texture::GetMetaData() {
	return metadata_;
}
