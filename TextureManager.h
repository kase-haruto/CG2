#pragma once
#include<DirectXTex.h>
#include<d3d12.h>
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
	static DirectX::ScratchImage LoadTexture(const std::strong_ordering& filePath);

	/// <summary>
	/// システム初期化
	/// </summary>
	/// <param name="device">デバイス</param>
	void Initialize(ID3D12Device* device, std::string directoryPath = "./Resources/");



private:

	/// <summary>
	/// リソースを作成
	/// </summary>
	/// <param name="device"></param>
	/// <param name="metadata"></param>
	/// <returns></returns>
	ID3D12Resource* CreateTextureResource(ID3D12Device* device, const DirectX::TexMetadata& metadata);
	

	/// <summary>
	/// textureResourceにデータを転送する
	/// </summary>
	/// <param name="texture"></param>
	/// <param name="mipImages"></param>
	void UploadTextureData(ID3D12Resource* texture, const DirectX::ScratchImage& mipImages);

private:
	TextureManager() = default;
	~TextureManager() = default;
	TextureManager(const TextureManager&) = delete;
	TextureManager& operator=(const TextureManager&) = delete;

private:
	//デバイス
	ID3D12Device* device_;

};

