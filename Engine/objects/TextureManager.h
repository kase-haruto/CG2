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

    /// <summary>
    /// 初期化
    /// </summary>
    /// <param name="imgui"></param>
    void Initialize(ImGuiManager* imgui);
    /// <summary>
    /// 終了処理
    /// </summary>
    void Finalize();

    /// <summary>
    /// textureの読み込み
    /// </summary>
    /// <param name="filePath"></param>
    /// <returns></returns>
    D3D12_GPU_DESCRIPTOR_HANDLE LoadTexture(const std::string& filePath);

    /// <summary>
    /// 起動時に読み込むテクスチャ
    /// </summary>
    void StartUpLoad();


    /// <summary>
    /// getter
    /// </summary>
    /// <returns></returns>
    ID3D12DescriptorHeap* GetDescriptorHeap() const;
    const DirectX::TexMetadata& GetMetaData(const std::string& filePath){ textures_.find(filePath)->second.GetMetaData(); }
    //handleの取得
    D3D12_GPU_DESCRIPTOR_HANDLE GetSrvHandle(const std::string& textureName) const;
    // 現在読み込まれているテクスチャのリストを取得
    const std::unordered_map<std::string, Texture>& GetLoadedTextures() const;
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
