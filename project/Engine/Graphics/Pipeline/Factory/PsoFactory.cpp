#include "PsoFactory.h"

#include <Engine/Graphics/Context/GraphicsGroup.h>

#include <stdexcept>

std::unique_ptr<PipelineStateObject> PsoFactory::Create(const GraphicsPipelineDesc& desc) {
	if (!shaderCompiler_) throw std::runtime_error("ShaderCompiler is null in PipelineFactory");


	ID3D12Device* device = GraphicsGroup::GetInstance()->GetDevice().Get();
	auto pipeline = std::make_unique<PipelineStateObject>();

	//  Root Signature ==================================================================//
	D3D12_ROOT_SIGNATURE_DESC rootDesc = desc.root_.Desc();
	ComPtr<ID3DBlob> sigBlob, errBlob;
	if (FAILED(D3D12SerializeRootSignature(&rootDesc, D3D_ROOT_SIGNATURE_VERSION_1,
										   &sigBlob, &errBlob))) {
		if (errBlob) OutputDebugStringA((char*)errBlob->GetBufferPointer());
		throw std::runtime_error("RootSignature serialization failed");
	}
	ComPtr<ID3D12RootSignature> rootSig;
	device->CreateRootSignature(0, sigBlob->GetBufferPointer(), sigBlob->GetBufferSize(),
								IID_PPV_ARGS(&rootSig));

   // psoDesc ==========================================================================//
	D3D12_GRAPHICS_PIPELINE_STATE_DESC pso{};
	pso.pRootSignature = rootSig.Get();
	pso.InputLayout = { desc.inputElems_.data(), (UINT)desc.inputElems_.size() };
	pso.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	pso.SampleDesc.Count = desc.sampleCount_;
	pso.NumRenderTargets = (UINT)desc.rtvFormats_.size();
	for (size_t i = 0; i < desc.rtvFormats_.size(); ++i)
		pso.RTVFormats[i] = desc.rtvFormats_[i];
	pso.DSVFormat = desc.dsvFormat_;
	pso.RasterizerState = desc.rasterizer_;
	pso.BlendState = desc.blend_;
	pso.DepthStencilState = desc.depth_;

	auto compile = [&](const std::wstring& path, const wchar_t* profile) {
		return shaderCompiler_->CompileShader(path, profile);
	};

	if (!desc.vs_.empty()) {
		auto vs = compile(desc.vs_, L"vs_6_0");
		pso.VS = { vs->GetBufferPointer(), vs->GetBufferSize() };
	}
	if (!desc.ps_.empty()) {
		auto ps = compile(desc.ps_, L"ps_6_0");
		pso.PS = { ps->GetBufferPointer(), ps->GetBufferSize() };
	}
	//if (!desc.cs_.empty()) {
	//    auto cs = compile(desc.cs_, L"cs_6_0");
	//    pso.CS = { cs->GetBufferPointer(), cs->GetBufferSize() };
	//}

	// Create PSO ==================================================================//
	pipeline->SetRootSignature(rootSig.Get());
	if (!pipeline->Initialize(pso))
		throw std::runtime_error("PipelineState initialization failed");

	return pipeline;
}
