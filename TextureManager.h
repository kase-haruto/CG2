#pragma once
#include<DirectXTex.h>
#include<d3d12.h>
#include<string>
#include <wrl.h>

class ImGuiManager;

class TextureManager{
public:

	/// <summary>
	/// シングルトンインスタンスの取得
	/// </summary>
	/// <returns>シングルトンインスタンス</returns>
	static TextureManager* GetInstance();

	/// <summary>
	/// Textureデータを読む
	/// </summary>
	/// <param name="filePath"></param>
	/// <returns></returns>
	static DirectX::ScratchImage LoadTexture(const std::string& filePath);

	/// <summary>
	/// システム初期化
	/// </summary>
	/// <param name="device">デバイス</param>
	void Initialize(ID3D12Device* device, ImGuiManager* imgui);

	/// <summary>
	/// リソースを作成
	/// </summary>
	/// <param name="device"></param>
	/// <param name="metadata"></param>
	/// <returns></returns>
	static ID3D12Resource* CreateTextureResource(ID3D12Device* device, const DirectX::TexMetadata& metadata);
	

	/// <summary>
	/// textureResourceにデータを転送する
	/// </summary>
	/// <param name="texture"></param>
	/// <param name="mipImages"></param>
	static void UploadTextureData(ID3D12Resource* texture, const DirectX::ScratchImage& mipImages);

	/// <summary>
	/// shaderResourceViewの作成
	/// </summary>
	/// <param name="texture"></param>
	/// <param name="metadata"></param>
	void CreateShaderResourceView(ID3D12Resource* texture, const DirectX::TexMetadata& metadata);

	D3D12_GPU_DESCRIPTOR_HANDLE GetTextureSrvHandle()const{ return textureSrvHandleGPU; }
private:


private:
	TextureManager() = default;
	~TextureManager(){ device_->Release(); }
	TextureManager(const TextureManager&) = delete;
	TextureManager& operator=(const TextureManager&) = delete;

private:
	//デバイス
	ID3D12Device* device_ = nullptr;
	ImGuiManager* imgui_;

	D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU;
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU;
};

