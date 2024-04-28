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
	//テクスチャファイルを読んでプログラムで扱えるようにする
	DirectX::ScratchImage image {};
	std::wstring filePathW = ConvertString(filePath);
	HRESULT hr = DirectX::LoadFromWICFile(filePathW.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);
	assert(SUCCEEDED(hr));

	//ミマップの作成
	DirectX::ScratchImage mipImages {};
	hr = DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::TEX_FILTER_SRGB, 0, mipImages);
	assert(SUCCEEDED(hr));

	//ミップマップ付きのデータを返す
	return mipImages;
}

ID3D12Resource* TextureManager::CreateTextureResource(ID3D12Device* device, const DirectX::TexMetadata& metadata){
	/*=============================================================
		metadataをもとにResourceの設定
	==============================================================*/
	D3D12_RESOURCE_DESC resourceDesc {};
	resourceDesc.Width = UINT(metadata.width);//textureの幅
	resourceDesc.Height = UINT(metadata.height);//textureの高さ
	resourceDesc.MipLevels = UINT16(metadata.mipLevels);//mipmapの数
	resourceDesc.DepthOrArraySize = UINT16(metadata.arraySize);//奥行き
	resourceDesc.Format = metadata.format;//textureのformat
	resourceDesc.SampleDesc.Count = 1;//サンプリングカウント
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION(metadata.dimension);//textureの次元数

	/*=============================================================
		利用するHeapの設定
	==============================================================*/
	//特殊な運用
	D3D12_HEAP_PROPERTIES heapProperties {};
	heapProperties.Type = D3D12_HEAP_TYPE_CUSTOM;//細かい設定を行う
	heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;//WriteBackポリシーでcpuアクセス可能
	heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;//プロセッサの近くに配置

	/*=============================================================
		Resourceの生成
	==============================================================*/
	ID3D12Resource* resource = nullptr;
	HRESULT hr = device->CreateCommittedResource(
		&heapProperties,//Heapの設定
		D3D12_HEAP_FLAG_NONE,//Heapの特殊な設定
		&resourceDesc,//Resourceの設定
		D3D12_RESOURCE_STATE_GENERIC_READ,//初回のResourceState。Textureは基本読むだけ
		nullptr,//clrar最適値
		IID_PPV_ARGS(&resource));//作成するesourceポインタへのポインタ
	assert(SUCCEEDED(hr));
	return resource;
}

void TextureManager::UploadTextureData(ID3D12Resource* texture, const DirectX::ScratchImage& mipImages){
	//Meta情報を取得
	const DirectX::TexMetadata& metadata = mipImages.GetMetadata();
	//全MipMapについて
	for (size_t mipLevel = 0; mipLevel < metadata.mipLevels; mipLevel++){
		//MipMapLevelを指定して各imageを取得
		const DirectX::Image* img = mipImages.GetImage(mipLevel, 0, 0);
		//Textureに転送
		HRESULT hr = texture->WriteToSubresource(
			UINT(mipLevel),
			nullptr,
			img->pixels,//元データアドレス
			UINT(img->rowPitch),//1ラインサイズ
			UINT(img->slicePitch)//1枚サイズ
		);
		assert(SUCCEEDED(hr));
	}
}