#include "PipelinePresets.h"

#include <Engine/Graphics/Pipeline/PipelineDesc/Input/VertexLayout.h>

/* ================================================================================================
/*							Objects
/* ================================================================================================ */

/////////////////////////////////////////////////////////////////////////////////////////
//		object3D
/////////////////////////////////////////////////////////////////////////////////////////
GraphicsPipelineDesc PipelinePresets::MakeObject3D(BlendMode mode){
	GraphicsPipelineDesc desc;
	desc.VS(L"Object3d.VS.hlsl")
		.PS(L"Object3d.PS.hlsl")
		.Input(VertexInputLayout<VertexPosUvN>::Get())
		.Blend(mode)
		.CullBack()
		.DepthEnable(true)
		.DepthFunc(D3D12_COMPARISON_FUNC_LESS_EQUAL)
		.RTV(DXGI_FORMAT_R8G8B8A8_UNORM)
		.Samples(1);

	desc.root_
		.AllowIA()
		.CBV(0, D3D12_SHADER_VISIBILITY_PIXEL)   // Material
		.CBV(0, D3D12_SHADER_VISIBILITY_VERTEX)  // WVP
		.SRVTable(0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, D3D12_SHADER_VISIBILITY_PIXEL) // Tex
		.CBV(2, D3D12_SHADER_VISIBILITY_PIXEL)   // DirLight
		.CBV(1, D3D12_SHADER_VISIBILITY_ALL)     // Camera
		.CBV(4, D3D12_SHADER_VISIBILITY_PIXEL)   // PointLight
		.SRVTable(1, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, D3D12_SHADER_VISIBILITY_PIXEL) // EnvMap
		.SamplerWrapLinear(0);

	return desc;
}

GraphicsPipelineDesc PipelinePresets::MakeSkinningObject3D(BlendMode mode){
	GraphicsPipelineDesc desc;
	desc.VS(L"SkinningObject3d.VS.hlsl")
		.PS(L"Object3d.PS.hlsl")
		.Input(VertexInputLayout<VertexPosUvNSkinning>::Get())
		.Blend(mode)
		.CullBack()
		.DepthEnable(true)
		.DepthFunc(D3D12_COMPARISON_FUNC_LESS_EQUAL)
		.RTV(DXGI_FORMAT_R8G8B8A8_UNORM)
		.Samples(1);

	desc.root_

		.AllowIA()
		.CBV(0, D3D12_SHADER_VISIBILITY_PIXEL)   // Material
		.CBV(0, D3D12_SHADER_VISIBILITY_VERTEX)  // WVP
		.SRVTable(0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, D3D12_SHADER_VISIBILITY_PIXEL) // Tex
		.CBV(2, D3D12_SHADER_VISIBILITY_PIXEL)   // DirLight
		.CBV(1, D3D12_SHADER_VISIBILITY_ALL)     // Camera
		.CBV(4, D3D12_SHADER_VISIBILITY_PIXEL)   // PointLight
		.SRVTable(1, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, D3D12_SHADER_VISIBILITY_PIXEL) // EnvMap
		.SRVTable(2, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, D3D12_SHADER_VISIBILITY_VERTEX) // SkinningBuffer
		.SamplerWrapLinear(0);

	return desc;
}

/////////////////////////////////////////////////////////////////////////////////////////
//		SkinObject3D
/////////////////////////////////////////////////////////////////////////////////////////


/* ================================================================================================
/*							postProcess
/* ================================================================================================ */

/////////////////////////////////////////////////////////////////////////////////////////
//		CopyImage
/////////////////////////////////////////////////////////////////////////////////////////
GraphicsPipelineDesc PipelinePresets::MakeCopyImage(){
	GraphicsPipelineDesc desc;
	desc.VS(L"CopyImage.VS.hlsl")
		.PS(L"CopyImage.PS.hlsl")
		.BlendNone()
		.CullNone()
		.DepthEnable(false)
		.RTV(DXGI_FORMAT_R8G8B8A8_UNORM)
		.Samples(1);

	desc.inputElems_.clear();

	desc.root_
		.AllowIA()
		.SRVTable(0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, D3D12_SHADER_VISIBILITY_PIXEL)
		.SampleClampLinear(0);
	return desc;
}

/////////////////////////////////////////////////////////////////////////////////////////
//		GrayScale
/////////////////////////////////////////////////////////////////////////////////////////
GraphicsPipelineDesc PipelinePresets::MakeGrayScale(){
	GraphicsPipelineDesc desc;
	desc.VS(L"CopyImage.VS.hlsl")
		.PS(L"GrayScale.PS.hlsl")
		.BlendNone()
		.CullNone()
		.DepthEnable(false)
		.RTV(DXGI_FORMAT_R8G8B8A8_UNORM)
		.Samples(1);

	desc.inputElems_.clear();

	desc.root_
		.AllowIA()
		.SRVTable(0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, D3D12_SHADER_VISIBILITY_PIXEL)
		.SampleClampLinear(0);

	return desc;
}

/////////////////////////////////////////////////////////////////////////////////////////
//		radialBlur
/////////////////////////////////////////////////////////////////////////////////////////
GraphicsPipelineDesc PipelinePresets::MakeRadialBlur(){
	GraphicsPipelineDesc desc;
	desc.VS(L"CopyImage.VS.hlsl")
		.PS(L"RadialBlur.PS.hlsl")
		.BlendNone()
		.CullNone()
		.DepthEnable(false)
		.RTV(DXGI_FORMAT_R8G8B8A8_UNORM)
		.Samples(1);

	desc.inputElems_.clear();

	desc.root_
		.AllowIA()
		.SRVTable(0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, D3D12_SHADER_VISIBILITY_PIXEL)
		.SampleClampLinear(0);
	return desc;
}
