#pragma once
#include <Engine/Assets/Texture/Texture.h>

/* c++ */
#include <unordered_map>

class ImGuiManager;

class TextureManager{
public:
	static TextureManager* GetInstance();

	void Initialize(ImGuiManager* imgui);
	void Finalize();
	D3D12_GPU_DESCRIPTOR_HANDLE LoadTexture(const std::string& filePath);
	void StartUpLoad();


	ID3D12DescriptorHeap* GetDescriptorHeap() const;
	const DirectX::TexMetadata& GetMetaData(const std::string& filePath){ textures_.find(filePath)->second.GetMetaData(); }
	D3D12_GPU_DESCRIPTOR_HANDLE GetSrvHandle(const std::string& textureName) const;
	const std::unordered_map<std::string, Texture>& GetLoadedTextures() const;

	void SetEnvironmentTexture(const std::string& filePath);
	D3D12_GPU_DESCRIPTOR_HANDLE GetEnvironmentTextureSrvHandle() const;
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

	std::string environmentTextureName_ = "environment";
	std::unordered_map<std::string, Texture> textures_;
};
