#pragma once
/* ========================================================================
/* 	include space
/* ===================================================================== */

#include <d3d12.h>
#include <vector>
#include <wrl.h>


class RootSignatureBuilder{
public:
	//===================================================================*/
	//		public functions
	//===================================================================*/
	// parmSet ----------------------------------------------------------//
	RootSignatureBuilder& CBV(D3D12_SHADER_VISIBILITY vis, UINT reg);
	RootSignatureBuilder& SRV(D3D12_SHADER_VISIBILITY vis, UINT reg);
	RootSignatureBuilder& UAV(D3D12_SHADER_VISIBILITY vis, UINT reg);

	// samplerSet -------------------------------------------------------//
	RootSignatureBuilder& StaticSampler(const D3D12_STATIC_SAMPLER_DESC& s);
	RootSignatureBuilder& SamplerWrapLinear(UINT reg,D3D12_SHADER_VISIBILITY vis = D3D12_SHADER_VISIBILITY_PIXEL);
	RootSignatureBuilder& SamplerWrapAniso(UINT reg, UINT maxAniso = 16, D3D12_SHADER_VISIBILITY vis = D3D12_SHADER_VISIBILITY_PIXEL);

	// flags -------------------------------------------------------------//
	RootSignatureBuilder& AllowIA();
	RootSignatureBuilder& DenyVS();
	RootSignatureBuilder& DenyPS();

	// build -------------------------------------------------------------//
	D3D12_ROOT_SIGNATURE_DESC Desc() const;

	// hash --------------------------------------------------------------//
	size_t Hash() const;
private:
	//===================================================================*/
	//		private functions
	//===================================================================*/
	static D3D12_ROOT_PARAMETER RootParm(D3D12_ROOT_PARAMETER_TYPE type, D3D12_SHADER_VISIBILITY vis, UINT reg);
	static D3D12_DESCRIPTOR_RANGE RootRange(D3D12_DESCRIPTOR_RANGE_TYPE type, UINT num, UINT baseReg);
	static D3D12_STATIC_SAMPLER_DESC makeSampler(UINT reg, D3D12_SHADER_VISIBILITY vis,
												 D3D12_FILTER filter,
												 D3D12_TEXTURE_ADDRESS_MODE addrMode);

	void AddParm(D3D12_ROOT_PARAMETER param);
	void AddTable(const D3D12_DESCRIPTOR_RANGE& r, D3D12_SHADER_VISIBILITY vis);



private:
	//===================================================================*/
	//		private variables
	//===================================================================*/
	std::vector<D3D12_ROOT_PARAMETER> params_;					//< ルートパラメータ
	std::vector<D3D12_STATIC_SAMPLER_DESC> samplers_;			//< スタティックサンプラ
	std::vector<D3D12_DESCRIPTOR_RANGE> descriptorRanges_;		//< レンジ
	D3D12_ROOT_SIGNATURE_FLAGS flags_ {};						//< フラグ
};

