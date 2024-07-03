#include "RootSignatureManager.h"
#include"DirectXCommon.h"
#include"ConvertString.h"

#include<format>
#include<cassert>

RootSignatureManager*RootSignatureManager::GetInstance(){
	static RootSignatureManager instance;
	return &instance;
}

void RootSignatureManager::Initialize(DirectXCommon* dxCommon){
	device_ = dxCommon->GetDevice();
	CreateRootSignature();
}

void RootSignatureManager::Update(){
	
}

void RootSignatureManager::CreateRootSignature(){
	HRESULT hr;

	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature {};
	descriptionRootSignature.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	descriptorRange[0].BaseShaderRegister = 0;//0から始まる
	descriptorRange[0].NumDescriptors = 1;//数は1つ
	descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;//srvを使用する
	descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;//offsetを自動計算

	//RootParamer作成
	D3D12_ROOT_PARAMETER rootParamenters[5] = {};

	//定数バッファをピクセルシェーダで使用
	rootParamenters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;  //cvbを使う
	rootParamenters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; //PixelShaderで使う
	rootParamenters[0].Descriptor.ShaderRegister = 0;

	//定数バッファをバーテックスシェーダで使用
	rootParamenters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;  //cvbを使う
	rootParamenters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;//vertexShaderで使う
	rootParamenters[1].Descriptor.ShaderRegister = 0;

	//フォグ定数バッファをピクセルシェーダで使用
	rootParamenters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParamenters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParamenters[2].Descriptor.ShaderRegister = 1;

	rootParamenters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//descriptorTableを使う
	rootParamenters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
	rootParamenters[3].DescriptorTable.pDescriptorRanges = descriptorRange;//tableの中身の配列を指定
	rootParamenters[3].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange);//Tableで利用する数

	//ライト
	rootParamenters[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParamenters[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParamenters[4].Descriptor.ShaderRegister = 2;

	descriptionRootSignature.pParameters = rootParamenters;
	descriptionRootSignature.NumParameters = _countof(rootParamenters);


	//samplerの設定
	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;//バイリニアフィルタ
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;//0~1の範囲外を利ぴ＝と
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;//ありったけのMipmapを使う
	staticSamplers[0].ShaderRegister = 0;//レジスタ番号0を使う
	staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//pixelShaderで使う
	descriptionRootSignature.pStaticSamplers = staticSamplers;
	descriptionRootSignature.NumStaticSamplers = _countof(staticSamplers);

	//シリアライズしてバイナリする
	hr = D3D12SerializeRootSignature(&descriptionRootSignature,
									 D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob_, &errorBlob_);
	if (FAILED(hr)){
		Log(reinterpret_cast< char* >(errorBlob_->GetBufferPointer()));
		assert(false);
	}

	//バイナリをもとに生成
	hr = device_->CreateRootSignature(0, signatureBlob_->GetBufferPointer(),
								    	 signatureBlob_->GetBufferSize(), IID_PPV_ARGS(&rootSignature_));
	assert(SUCCEEDED(hr));
}