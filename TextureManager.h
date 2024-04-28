#pragma once
#include<DirectXTex.h>
#include<d3d12.h>
class TextureManager{
public:

	/// <summary>
	/// �V���O���g���C���X�^���X�̎擾
	/// </summary>
	/// <returns>�V���O���g���C���X�^���X</returns>
	static TextureManager* GetInstance();

	/// <summary>
	/// Texture�f�[�^��ǂ�
	/// </summary>
	/// <param name="filePath"></param>
	/// <returns></returns>
	static DirectX::ScratchImage LoadTexture(const std::strong_ordering& filePath);

	/// <summary>
	/// �V�X�e��������
	/// </summary>
	/// <param name="device">�f�o�C�X</param>
	void Initialize(ID3D12Device* device, std::string directoryPath = "./Resources/");



private:

	/// <summary>
	/// ���\�[�X���쐬
	/// </summary>
	/// <param name="device"></param>
	/// <param name="metadata"></param>
	/// <returns></returns>
	ID3D12Resource* CreateTextureResource(ID3D12Device* device, const DirectX::TexMetadata& metadata);
	

	/// <summary>
	/// textureResource�Ƀf�[�^��]������
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
	//�f�o�C�X
	ID3D12Device* device_;

};

