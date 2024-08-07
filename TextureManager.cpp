﻿#include "TextureManager.h"
#include"ConvertString.h"
#include"ImGuiManager.h"
#include"DirectXCommon.h"
#include"MyFunc.h"

TextureManager* TextureManager::GetInstance(){
	static TextureManager instance;
	return &instance;
}
std::vector<std::string> TextureManager::texturePath_;
void TextureManager::Initialize(ID3D12Device* device, ImGuiManager* imgui){
	dxCommon_ = DirectXCommon::GetInstance();
	device_ = device;
	imgui_ = imgui;
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


DirectX::ScratchImage TextureManager::LoadTexture(const std::string& filePath){
	//配列にファイルパスを格納
	texturePath_.push_back(filePath);

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


void TextureManager::TransferTexture(){
	DirectX::ScratchImage mipImages = TextureManager::LoadTexture("./Resources/uvChecker.png");
	const DirectX::TexMetadata& metadata = mipImages.GetMetadata();
	textureResource = TextureManager::CreateTextureResource(dxCommon_->GetDevice(), metadata);
	TextureManager::UploadTextureData(textureResource.Get(), mipImages);

	//2枚目のtextureを読んで転送する
	DirectX::ScratchImage mipImages2 = LoadTexture("./Resources/checkerBoard.png");
	const DirectX::TexMetadata& metadata2 = mipImages2.GetMetadata();
	textureResource2 = CreateTextureResource(dxCommon_->GetDevice(), metadata2);
	UploadTextureData(textureResource2.Get(), mipImages2);

	TextureManager::GetInstance()->CreateShaderResourceView(textureResource.Get(), metadata, metadata2);
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

void TextureManager::CreateShaderResourceView(ID3D12Resource* texture,
											  const DirectX::TexMetadata& metadata,
											  const DirectX::TexMetadata& metadata2){
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvHeap = imgui_->GetSrvHeap();
	//metaDataをもとにsrvの設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc {};
	srvDesc.Format = metadata.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2dテクスチャ
	srvDesc.Texture2D.MipLevels = UINT(metadata.mipLevels);

	////srvを作成するdescriptorHeapの場所を決める
	//textureSrvHandleCPU = imgui_->GetSrvHeap()->GetCPUDescriptorHandleForHeapStart();
	//textureSrvHandleGPU = imgui_->GetSrvHeap()->GetGPUDescriptorHandleForHeapStart();

	////先頭はImGuiが使用しているので次のを使う
	//textureSrvHandleCPU.ptr += device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	//textureSrvHandleGPU.ptr += device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	textureSrvHandleCPU = GetCPUDescriptorHandle(srvHeap.Get(), dxCommon_->GetDescriptorSizeSRV(), 1);
	textureSrvHandleGPU = GetGPUDescriptorHandle(srvHeap.Get(), dxCommon_->GetDescriptorSizeSRV(), 1);

	//srvの生成
	device_->CreateShaderResourceView(texture, &srvDesc, textureSrvHandleCPU);

	//=================================
	//2枚目
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc2 {};
	srvDesc2.Format = metadata2.format;
	srvDesc2.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc2.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc2.Texture2D.MipLevels = UINT(metadata2.mipLevels);

	//srvを生成するdescriptorHeapの場所を決める
	textureSrvHandleCPU2 = GetCPUDescriptorHandle(srvHeap.Get(), dxCommon_->GetDescriptorSizeSRV(), 2);
	textureSrvHandleGPU2 = GetGPUDescriptorHandle(srvHeap.Get(), dxCommon_->GetDescriptorSizeSRV(), 2);

	device_->CreateShaderResourceView(textureResource2.Get(), &srvDesc2, textureSrvHandleCPU2);
}