#include "TextureManager.h"
#include<string>
#include"ConvertString.h"

TextureManager* TextureManager::GetInstance(){
	static TextureManager instance;
	return &instance;
}

void TextureManager::Initialize(ID3D12Device* device, std::string directoryPath){
	device_ = device;
}

DirectX::ScratchImage TextureManager::LoadTexture(const std::strong_ordering& filePath){
	//�e�N�X�`���t�@�C����ǂ�Ńv���O�����ň�����悤�ɂ���
	DirectX::ScratchImage image {};
	std::wstring filePathW = ConvertString(filePath);
	HRESULT hr = DirectX::LoadFromWICFile(filePathW.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);
	assert(SUCCEEDED(hr));

	//�~�}�b�v�̍쐬
	DirectX::ScratchImage mipImages {};
	hr = DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::TEX_FILTER_SRGB, 0, mipImages);
	assert(SUCCEEDED(hr));

	//�~�b�v�}�b�v�t���̃f�[�^��Ԃ�
	return mipImages;
}

ID3D12Resource* TextureManager::CreateTextureResource(ID3D12Device* device, const DirectX::TexMetadata& metadata){
	/*=============================================================
		metadata�����Ƃ�Resource�̐ݒ�
	==============================================================*/
	D3D12_RESOURCE_DESC resourceDesc {};
	resourceDesc.Width = UINT(metadata.width);//texture�̕�
	resourceDesc.Height = UINT(metadata.height);//texture�̍���
	resourceDesc.MipLevels = UINT16(metadata.mipLevels);//mipmap�̐�
	resourceDesc.DepthOrArraySize = UINT16(metadata.arraySize);//���s��
	resourceDesc.Format = metadata.format;//texture��format
	resourceDesc.SampleDesc.Count = 1;//�T���v�����O�J�E���g
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION(metadata.dimension);//texture�̎�����

	/*=============================================================
		���p����Heap�̐ݒ�
	==============================================================*/
	//����ȉ^�p
	D3D12_HEAP_PROPERTIES heapProperties {};
	heapProperties.Type = D3D12_HEAP_TYPE_CUSTOM;//�ׂ����ݒ���s��
	heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;//WriteBack�|���V�[��cpu�A�N�Z�X�\
	heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;//�v���Z�b�T�̋߂��ɔz�u

	/*=============================================================
		Resource�̐���
	==============================================================*/
	ID3D12Resource* resource = nullptr;
	HRESULT hr = device->CreateCommittedResource(
		&heapProperties,//Heap�̐ݒ�
		D3D12_HEAP_FLAG_NONE,//Heap�̓���Ȑݒ�
		&resourceDesc,//Resource�̐ݒ�
		D3D12_RESOURCE_STATE_GENERIC_READ,//�����ResourceState�BTexture�͊�{�ǂނ���
		nullptr,//clrar�œK�l
		IID_PPV_ARGS(&resource));//�쐬����esource�|�C���^�ւ̃|�C���^
	assert(SUCCEEDED(hr));
	return resource;
}

void TextureManager::UploadTextureData(ID3D12Resource* texture, const DirectX::ScratchImage& mipImages){
	//Meta�����擾
	const DirectX::TexMetadata& metadata = mipImages.GetMetadata();
	//�SMipMap�ɂ���
	for (size_t mipLevel = 0; mipLevel < metadata.mipLevels; mipLevel++){
		//MipMapLevel���w�肵�Ċeimage���擾
		const DirectX::Image* img = mipImages.GetImage(mipLevel, 0, 0);
		//Texture�ɓ]��
		HRESULT hr = texture->WriteToSubresource(
			UINT(mipLevel),
			nullptr,
			img->pixels,//���f�[�^�A�h���X
			UINT(img->rowPitch),//1���C���T�C�Y
			UINT(img->slicePitch)//1���T�C�Y
		);
		assert(SUCCEEDED(hr));
	}
}