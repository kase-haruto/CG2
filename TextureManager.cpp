#include "TextureManager.h"
#include "GraphicsGroup.h"
#include "ImGuiManager.h"
#include <cassert>
#include"SrvLocator.h"
#include"SrvLocator.h"

TextureManager* TextureManager::GetInstance(){
    static TextureManager instance;
    return &instance;
}

void TextureManager::Initialize(ImGuiManager* imgui){
    device_ = GraphicsGroup::GetInstance()->GetDevice();
    imgui_ = imgui;
    descriptorSizeSrv_ = device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

void TextureManager::Finalize(){
    for (auto& texture : textures_){
        texture.second.~Texture();
    }
    device_.Reset();
}

D3D12_GPU_DESCRIPTOR_HANDLE TextureManager::LoadTexture(const std::string& filePath){
    // テクスチャが既にロードされているか確認
    if (textures_.find(filePath) != textures_.end()){
        return textures_[filePath].GetSrvHandle();
    }

    // 新しいテクスチャをロード
    Texture texture(filePath);
    texture.Load(device_.Get());
    texture.Upload(device_.Get());

    // シェーダリソースビューを作成
    texture.CreateShaderResourceView(device_.Get());

    // SRVハンドルを取得し、マップに追加
    D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle = texture.GetSrvHandle();
    textures_[filePath] = std::move(texture);

    return gpuHandle;
}
