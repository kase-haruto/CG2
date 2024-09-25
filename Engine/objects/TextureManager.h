#pragma once

#include <d3d12.h>
#include <wrl.h>
#include <unordered_map>
#include <string>
#include "Texture.h"

class ImGuiManager;

class TextureManager{
public:
    static TextureManager* GetInstance();

    void Initialize(ImGuiManager* imgui);
    void Finalize();

    D3D12_GPU_DESCRIPTOR_HANDLE LoadTexture(const std::string& filePath);
    ID3D12DescriptorHeap* GetDescriptorHeap() const;

private:
    TextureManager() = default;
    ~TextureManager() = default;
    TextureManager(const TextureManager&) = delete;
    TextureManager& operator=(const TextureManager&) = delete;

private:
    Microsoft::WRL::ComPtr<ID3D12Device> device_ = nullptr;
    ImGuiManager* imgui_ = nullptr;
    uint32_t descriptorSizeSrv_ = 0;
    uint32_t descriptorHeapIndex_ = 1;

    std::unordered_map<std::string, Texture> textures_;
};
