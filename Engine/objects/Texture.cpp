#include "Texture.h"
#include "ConvertString.h"
#include <cassert>
#include"SrvLocator.h"
#include"MyFunc.h"

Texture::Texture(const std::string& filePath) : filePath_(filePath){}

Texture::~Texture(){
    // リソースの解放処理
    resource_.Reset();
}

Texture::Texture(Texture&& other) noexcept
    : filePath_(std::move(other.filePath_)),
    image_(std::move(other.image_)),
    metadata_(std::move(other.metadata_)),
    resource_(std::move(other.resource_)),
    srvHandleCPU_(other.srvHandleCPU_),
    srvHandleGPU_(other.srvHandleGPU_){
    other.srvHandleCPU_.ptr = 0;
    other.srvHandleGPU_.ptr = 0;
}

Texture& Texture::operator=(Texture&& other) noexcept{
    if (this != &other){
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

void Texture::Load(ID3D12Device* device){
    std::wstring filePathW = ConvertString("Resources/textures/" + filePath_);
    HRESULT hr = DirectX::LoadFromWICFile(filePathW.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image_);
    assert(SUCCEEDED(hr));

    image_ = LoadTextureImage("Resources/textures/" + filePath_);
    metadata_ = image_.GetMetadata();
}

void Texture::Upload(ID3D12Device* device){
    D3D12_RESOURCE_DESC resourceDesc = {};
    resourceDesc.Width = UINT(metadata_.width);
    resourceDesc.Height = UINT(metadata_.height);
    resourceDesc.MipLevels = UINT16(metadata_.mipLevels);
    resourceDesc.DepthOrArraySize = UINT16(metadata_.arraySize);
    resourceDesc.Format = metadata_.format;
    resourceDesc.SampleDesc.Count = 1;
    resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION(metadata_.dimension);

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
        IID_PPV_ARGS(&resource_));
    assert(SUCCEEDED(hr));

    for (size_t mipLevel = 0; mipLevel < metadata_.mipLevels; mipLevel++){
        const DirectX::Image* img = image_.GetImage(mipLevel, 0, 0);
        hr = resource_->WriteToSubresource(
            UINT(mipLevel),
            nullptr,
            img->pixels,
            UINT(img->rowPitch),
            UINT(img->slicePitch)
        );
        assert(SUCCEEDED(hr));
    }
}

void Texture::CreateShaderResourceView(ID3D12Device* device){
    auto srvHandle = SrvLocator::AllocateSrv();
    srvHandleCPU_ = srvHandle.first;
    srvHandleGPU_ = srvHandle.second;

    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format = metadata_.format;
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = UINT(metadata_.mipLevels);

    device->CreateShaderResourceView(resource_.Get(), &srvDesc, srvHandleCPU_);
}
