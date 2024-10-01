#include "System.h"
#include"TextureManager.h"
#include"GraphicsGroup.h"
#include"SrvLocator.h"

System::System(){}

System::~System(){}

void System::Initialize(int32_t clientWidth, int32_t clientHeight){
	winApp_ = std::make_unique<WinApp>();
	dxCommon_ = std::make_unique<DirectXCommon>();
	dxCommon_->Initialize(winApp_.get(), 1280, 720);
    device_ = dxCommon_->GetDevice();

	//管理クラスの初期化
    shaderManager_ = std::make_shared<ShaderManager>();
	pipelineStateManager_ = std::make_unique<PipelineStateManager>(device_,shaderManager_);

	//パイプラインを設定
	CreatePipelines();

    GraphicsGroup::GetInstance()->Initialize(dxCommon_.get(), pipelineStateManager_.get());

#ifdef _DEBUG
    imguiManager_ = std::make_unique<ImGuiManager>();
	imguiManager_->Initialize(winApp_.get(), dxCommon_.get());
#endif // _DEBUG

	//textureManagerの初期化
	TextureManager::GetInstance()->Initialize(imguiManager_.get());

    //////////////////////////////////////////////////////////////////////
    ///             ライトの初期化
    //////////////////////////////////////////////////////////////////////

    //directionalLight
    directionalLight_ = std::make_unique<DirectionalLight>();
    directionalLight_->Initialize(dxCommon_.get());
    directionalLight_->SetRootSignature(pipelineStateManager_->GetRootSignature(Object3D));

    //pointLight
    pointLight_ = std::make_unique<PointLight>();
    pointLight_->Initialize(dxCommon_.get());
    pointLight_->SetRootSignature(pipelineStateManager_->GetRootSignature(Object3D));
}

void System::BeginFrame(){
	//フレームの開始
    dxCommon_->PreDraw();
	// ImGui受付開始
	imguiManager_->Begin();
    //ライトの処理の更新
    directionalLight_->Render();
    pointLight_->Render();
}

void System::EndFrame(){
	//imguiのコマンドを積む
	imguiManager_->End();
	//ImGui描画
	imguiManager_->Draw();
	//フレームの終了
	dxCommon_->PostDraw();
}

void System::Finalize(){
    device_.Reset();
	imguiManager_->Finalize();
    TextureManager::GetInstance()->Finalize();
    pipelineStateManager_->Finalize();
    directionalLight_.reset();
    pointLight_.reset();
    SrvLocator::Finalize();

	//ウィンドウの破棄
	winApp_->TerminateGameWindow();
    dxCommon_->Finalize();
}


int System::ProcessMessage(){ return winApp_->ProcessMessage() ? 1 : 0; }



//=============================================================================================================
//              Pipelineの作成
//=============================================================================================================

void System::CreatePipelines(){
	  shaderManager_->InitializeDXC();
      Object3DPipelines();
      StructuredObjectPipeline();
}

void System::Object3DPipelines(){
    // InputLayoutの設定
    D3D12_INPUT_ELEMENT_DESC inputElementDescs[3] = {};
    inputElementDescs[0].SemanticName = "POSITION";
    inputElementDescs[0].SemanticIndex = 0;
    inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
    inputElementDescs[1].SemanticName = "TEXCOORD";
    inputElementDescs[1].SemanticIndex = 0;
    inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
    inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
    inputElementDescs[2].SemanticName = "NORMAL";
    inputElementDescs[2].SemanticIndex = 0;
    inputElementDescs[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
    inputElementDescs[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

    D3D12_INPUT_LAYOUT_DESC inputLayoutDesc {};
    inputLayoutDesc.pInputElementDescs = inputElementDescs;
    inputLayoutDesc.NumElements = _countof(inputElementDescs);

    // BlendStateの設定
    D3D12_BLEND_DESC blendDesc {};
    blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
    blendDesc.RenderTarget[0].BlendEnable = true;
    blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
    blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
    blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
    blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;

    // RasterizerStateの設定
    D3D12_RASTERIZER_DESC rasterizeDesc {};
    rasterizeDesc.CullMode = D3D12_CULL_MODE_NONE;
    rasterizeDesc.FillMode = D3D12_FILL_MODE_SOLID;

    // DepthStencilStateの設定
    D3D12_DEPTH_STENCIL_DESC depthStencilDesc {};
    depthStencilDesc.DepthEnable = true;
    depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
    depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

    // シェーダの読み込み
    if (!shaderManager_->LoadShader(Object3D, L"Object3d.VS.hlsl", L"Object3d.PS.hlsl")){
        // シェーダの読み込みに失敗した場合のエラーハンドリング
        return;
    }

    // RootSignatureの設定
    D3D12_ROOT_PARAMETER rootParameters[7] = {};
    D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
    descriptorRange[0].BaseShaderRegister = 0;
    descriptorRange[0].NumDescriptors = 1;
    descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
    descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

    //マテリアル
    rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
    rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
    rootParameters[0].Descriptor.ShaderRegister = 0;

    //wvp/world
    rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
    rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
    rootParameters[1].Descriptor.ShaderRegister = 0;

    //フォグ
    rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
    rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
    rootParameters[2].Descriptor.ShaderRegister = 1;

    //テクスチャ
    rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
    rootParameters[3].DescriptorTable.pDescriptorRanges = descriptorRange;
    rootParameters[3].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange);

    //DirectionalLight
    rootParameters[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
    rootParameters[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
    rootParameters[4].Descriptor.ShaderRegister = 2;

    //カメラ
    rootParameters[5].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
    rootParameters[5].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
    rootParameters[5].Descriptor.ShaderRegister = 3;

    //pointLight
    rootParameters[6].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
    rootParameters[6].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
    rootParameters[6].Descriptor.ShaderRegister = 4;

    D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc = {};
    rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
    rootSignatureDesc.pParameters = rootParameters;
    rootSignatureDesc.NumParameters = _countof(rootParameters);
    
    D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
    staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
    staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
    staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;
    staticSamplers[0].ShaderRegister = 0;
    staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

    rootSignatureDesc.pStaticSamplers = staticSamplers;
    rootSignatureDesc.NumStaticSamplers = _countof(staticSamplers);

    // RootSignatureの作成
    ComPtr<ID3DBlob> signatureBlob;
    ComPtr<ID3DBlob> errorBlob;
    HRESULT hr = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
    if (FAILED(hr)){
        // RootSignatureのシリアライズに失敗した場合のエラーハンドリング
        if (errorBlob){
            OutputDebugStringA(( char* ) errorBlob->GetBufferPointer());
        }
        return;
    }

    ComPtr<ID3D12RootSignature> rootSignature;
    hr = device_->CreateRootSignature(0, signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature));
    if (FAILED(hr)){
        // RootSignatureの作成に失敗した場合のエラーハンドリング
        return;
    }

    // PSOの設定
    D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
    psoDesc.pRootSignature = rootSignature.Get();
    psoDesc.InputLayout = inputLayoutDesc;
    psoDesc.VS = {shaderManager_->GetVertexShader(Object3D)->GetBufferPointer(), shaderManager_->GetVertexShader(Object3D)->GetBufferSize()};
    psoDesc.PS = {shaderManager_->GetPixelShader(Object3D)->GetBufferPointer(), shaderManager_->GetPixelShader(Object3D)->GetBufferSize()};
    psoDesc.BlendState = blendDesc;
    psoDesc.RasterizerState = rasterizeDesc;
    psoDesc.DepthStencilState = depthStencilDesc;
    psoDesc.NumRenderTargets = 1;
    psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
    psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    psoDesc.SampleDesc.Count = 1;
    psoDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
    psoDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

    // パイプラインステートオブジェクトの作成
    if (!pipelineStateManager_->CreatePipelineState(Object3D, L"Object3d.VS.hlsl", L"Object3d.PS.hlsl", rootSignatureDesc, psoDesc)){
        // エラーハンドリング
        return;
    }
}


void System::StructuredObjectPipeline(){
    // InputLayoutの設定
    D3D12_INPUT_ELEMENT_DESC inputElementDescs[3] = {};
    inputElementDescs[0].SemanticName = "POSITION";
    inputElementDescs[0].SemanticIndex = 0;
    inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
    inputElementDescs[1].SemanticName = "TEXCOORD";
    inputElementDescs[1].SemanticIndex = 0;
    inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
    inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
    inputElementDescs[2].SemanticName = "NORMAL";
    inputElementDescs[2].SemanticIndex = 0;
    inputElementDescs[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
    inputElementDescs[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

    D3D12_INPUT_LAYOUT_DESC inputLayoutDesc {};
    inputLayoutDesc.pInputElementDescs = inputElementDescs;
    inputLayoutDesc.NumElements = _countof(inputElementDescs);

    // BlendStateの設定
   // BlendStateの設定
    D3D12_BLEND_DESC blendDesc {};
    blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
    blendDesc.RenderTarget[0].BlendEnable = true;
    blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
    blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE; // 加算ブレンド
    blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD; // カラーの加算

    blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
    blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ONE; // アルファ値の加算
    blendDesc.RenderTarget[0].BlendOpAlpha= D3D12_BLEND_OP_ADD; // アルファブレンドの加算


    // RasterizerStateの設定
    D3D12_RASTERIZER_DESC rasterizeDesc {};
    rasterizeDesc.CullMode = D3D12_CULL_MODE_NONE;
    rasterizeDesc.FillMode = D3D12_FILL_MODE_SOLID;

    // DepthStencilStateの設定
    D3D12_DEPTH_STENCIL_DESC depthStencilDesc {};
    depthStencilDesc.DepthEnable = true;
    depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
    depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

    // シェーダの読み込み
    if (!shaderManager_->LoadShader(StructuredObject, L"Particle.VS.hlsl", L"Particle.PS.hlsl")){
        // シェーダの読み込みに失敗した場合のエラーハンドリング
        return;
    }

    // RootSignatureの設定
    D3D12_ROOT_PARAMETER rootParameters[4] = {};
    D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
    descriptorRange[0].BaseShaderRegister = 0;
    descriptorRange[0].NumDescriptors = 1;
    descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
    descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

    rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
    rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
    rootParameters[0].Descriptor.ShaderRegister = 0;

    rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
    rootParameters[1].DescriptorTable.pDescriptorRanges = descriptorRange;
    rootParameters[1].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange);


    rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
    rootParameters[2].DescriptorTable.pDescriptorRanges = descriptorRange;
    rootParameters[2].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange);


    D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc = {};
    rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
    rootSignatureDesc.pParameters = rootParameters;
    rootSignatureDesc.NumParameters = _countof(rootParameters);

    D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
    staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
    staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
    staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;
    staticSamplers[0].ShaderRegister = 0;
    staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

    rootSignatureDesc.pStaticSamplers = staticSamplers;
    rootSignatureDesc.NumStaticSamplers = _countof(staticSamplers);

    // RootSignatureの作成
    ComPtr<ID3DBlob> signatureBlob;
    ComPtr<ID3DBlob> errorBlob;
    HRESULT hr = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
    if (FAILED(hr)){
        // RootSignatureのシリアライズに失敗した場合のエラーハンドリング
        if (errorBlob){
            OutputDebugStringA(( char* ) errorBlob->GetBufferPointer());
        }
        return;
    }

    ComPtr<ID3D12RootSignature> rootSignature;
    hr = device_->CreateRootSignature(0, signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature));
    if (FAILED(hr)){
        // RootSignatureの作成に失敗した場合のエラーハンドリング
        return;
    }

    // PSOの設定
    D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
    psoDesc.pRootSignature = rootSignature.Get();
    psoDesc.InputLayout = inputLayoutDesc;
    psoDesc.VS = {shaderManager_->GetVertexShader(StructuredObject)->GetBufferPointer(), shaderManager_->GetVertexShader(StructuredObject)->GetBufferSize()};
    psoDesc.PS = {shaderManager_->GetPixelShader(StructuredObject)->GetBufferPointer(), shaderManager_->GetPixelShader(StructuredObject)->GetBufferSize()};
    psoDesc.BlendState = blendDesc;
    psoDesc.RasterizerState = rasterizeDesc;
    psoDesc.DepthStencilState = depthStencilDesc;
    psoDesc.NumRenderTargets = 1;
    psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
    psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    psoDesc.SampleDesc.Count = 1;
    psoDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
    psoDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

    // パイプラインステートオブジェクトの作成
    if (!pipelineStateManager_->CreatePipelineState(StructuredObject, L"Particle.VS.hlsl", L"Particle.PS.hlsl", rootSignatureDesc, psoDesc)){
        // エラーハンドリング
        return;
    }
}


