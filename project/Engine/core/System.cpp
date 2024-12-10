#include "../core/System.h"
#include "../objects/TextureManager.h"
#include "../graphics/GraphicsGroup.h"
#include "../graphics/SrvLocator.h"
#include "../core/Input.h"
#include "../objects/ModelManager.h"
#include "../graphics/camera/CameraManager.h"

HINSTANCE System::hInstance_ = nullptr;
HWND System::hwnd_ = nullptr;

System::System(){}


void System::Initialize(HINSTANCE hInstance, int32_t clientWidth, int32_t clientHeight,const std::string windowTitle){
    winApp_ = std::make_unique<WinApp>(clientWidth,clientHeight, windowTitle);
    hInstance_ = hInstance;
    hwnd_ = winApp_->GetHWND();

    dxCore_ = std::make_unique<DxCore>();
    dxCore_->Initialize(winApp_.get(), clientWidth, clientHeight);

    ComPtr<ID3D12Device> device = dxCore_->GetDevice();

    //インプットの初期化
    Input::Initialize();

	//管理クラスの初期化
    shaderManager_ = std::make_shared<ShaderManager>();
	pipelineStateManager_ = std::make_unique<PipelineStateManager>(device,shaderManager_);

	//パイプラインを設定
	CreatePipelines();

    GraphicsGroup::GetInstance()->Initialize(dxCore_.get(), pipelineStateManager_.get());

    imguiManager_ = std::make_unique<ImGuiManager>();
	imguiManager_->Initialize(winApp_.get(), dxCore_.get());

    // カメラの生成
    CameraManager::Initialize();

    //モデル管理クラスの初期化(インスタンス生成)
    ModelManager::Initialize();
    ModelManager::StartUpLoad();

	//textureManagerの初期化
	TextureManager::GetInstance()->Initialize(imguiManager_.get());
    //スタート時に読み込み
    TextureManager::GetInstance()->StartUpLoad();
}

void System::BeginFrame(){
	//フレームの開始
    dxCore_->PreDrawOffscreen();
	// ImGui受付開始
	imguiManager_->Begin();
    //インプットの更新
    Input::Update();
}

void System::EndFrame(){
    //オフスクリーンの内容をスワップチェーンに転送
    dxCore_->PreDraw();

	//imguiのコマンドを積む
	imguiManager_->End();
	//ImGui描画
	imguiManager_->Draw();
	//フレームの終了
    dxCore_->PostDraw();
}

void System::Finalize(){
    //imgui終了処理
	imguiManager_->Finalize();
    //textureの終了処理
    TextureManager::GetInstance()->Finalize();
    //モデルマネージャーの開放
    ModelManager::GetInstance()->Finalize();
    //カメラの開放
    CameraManager::Finalize();
    //pipelineの終了処理
    pipelineStateManager_->Finalize();
    SrvLocator::Finalize();
    Input::Finalize();


	//ウィンドウの破棄
	winApp_->TerminateGameWindow();
}


int System::ProcessMessage(){ return winApp_->ProcessMessage() ? 1 : 0; }



//=============================================================================================================
//              Pipelineの作成
//=============================================================================================================

void System::CreatePipelines(){
	  shaderManager_->InitializeDXC();
      Object3DPipelines();
      Object2DPipelines();
      StructuredObjectPipeline();
      LinePipeline();
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
    rootParameters[2].Descriptor.ShaderRegister = 5;

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
    ComPtr<ID3D12Device> device = dxCore_->GetDevice();
    hr = device->CreateRootSignature(0, signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature));
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

void System::Object2DPipelines(){
    // InputLayoutの設定 (2D用: POSITIONとTEXCOORDのみ)
    D3D12_INPUT_ELEMENT_DESC inputElementDescs[2] = {};
    inputElementDescs[0].SemanticName = "POSITION";
    inputElementDescs[0].SemanticIndex = 0;
    inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
    inputElementDescs[1].SemanticName = "TEXCOORD";
    inputElementDescs[1].SemanticIndex = 0;
    inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
    inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

    D3D12_INPUT_LAYOUT_DESC inputLayoutDesc {};
    inputLayoutDesc.pInputElementDescs = inputElementDescs;
    inputLayoutDesc.NumElements = _countof(inputElementDescs);

    // BlendStateの設定 (アルファブレンド有効)
    D3D12_BLEND_DESC blendDesc {};
    blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
    blendDesc.RenderTarget[0].BlendEnable = true;
    blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
    blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
    blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
    blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;

    // RasterizerStateの設定 (カリングなし)
    D3D12_RASTERIZER_DESC rasterizeDesc {};
    rasterizeDesc.CullMode = D3D12_CULL_MODE_NONE;
    rasterizeDesc.FillMode = D3D12_FILL_MODE_SOLID;

    // DepthStencilStateの設定 (深度ステンシルは無効化)
    D3D12_DEPTH_STENCIL_DESC depthStencilDesc {};
    depthStencilDesc.DepthEnable = false;
    depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
    depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS;

    // シェーダの読み込み
    if (!shaderManager_->LoadShader(Object2D, L"Object2d.VS.hlsl", L"Object2d.PS.hlsl")){
        // シェーダの読み込みに失敗した場合のエラーハンドリング
        return;
    }

    // RootSignatureの設定
    D3D12_ROOT_PARAMETER rootParameters[3] = {};

    // マテリアル
    rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
    rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
    rootParameters[0].Descriptor.ShaderRegister = 0;

    // wvp行列
    rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
    rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
    rootParameters[1].Descriptor.ShaderRegister = 0;

    D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
    descriptorRange[0].BaseShaderRegister = 0;
    descriptorRange[0].NumDescriptors = 1;
    descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
    descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

    // テクスチャ
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
    ComPtr<ID3D12Device> device = dxCore_->GetDevice();
    hr = device->CreateRootSignature(0, signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature));
    if (FAILED(hr)){
        // RootSignatureの作成に失敗した場合のエラーハンドリング
        return;
    }

    // PSOの設定
    D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
    psoDesc.pRootSignature = rootSignature.Get();
    psoDesc.InputLayout = inputLayoutDesc;
    psoDesc.VS = {shaderManager_->GetVertexShader(Object2D)->GetBufferPointer(), shaderManager_->GetVertexShader(Object2D)->GetBufferSize()};
    psoDesc.PS = {shaderManager_->GetPixelShader(Object2D)->GetBufferPointer(), shaderManager_->GetPixelShader(Object2D)->GetBufferSize()};
    psoDesc.BlendState = blendDesc;
    psoDesc.RasterizerState = rasterizeDesc;
    psoDesc.DepthStencilState = depthStencilDesc;
    psoDesc.NumRenderTargets = 1;
    psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
    psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    psoDesc.SampleDesc.Count = 1;
    psoDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
    psoDesc.DSVFormat = DXGI_FORMAT_UNKNOWN; // 深度ステンシル不要なのでDSVFormatは設定しない

    // パイプラインステートオブジェクトの作成
    if (!pipelineStateManager_->CreatePipelineState(Object2D, L"Object2d.VS.hlsl", L"Object2d.PS.hlsl", rootSignatureDesc, psoDesc)){
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
    D3D12_ROOT_PARAMETER rootParameters[3] = {};
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
    ComPtr<ID3D12Device> device = dxCore_->GetDevice();
    hr = device->CreateRootSignature(0, signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature));
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

void System::LinePipeline(){
    // InputLayoutの設定
    D3D12_INPUT_ELEMENT_DESC inputElementDescs[2] = {};
    inputElementDescs[0].SemanticName = "POSITION";
    inputElementDescs[0].SemanticIndex = 0;
    inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
    inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

    inputElementDescs[1].SemanticName = "COLOR";
    inputElementDescs[1].SemanticIndex = 0;
    inputElementDescs[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

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
    rasterizeDesc.CullMode = D3D12_CULL_MODE_NONE; // カリングを無効化
    rasterizeDesc.FillMode = D3D12_FILL_MODE_WIREFRAME; // 線描画の場合はこちら
    rasterizeDesc.FrontCounterClockwise = FALSE;
    rasterizeDesc.DepthClipEnable = TRUE;

    // DepthStencilStateの設定
    D3D12_DEPTH_STENCIL_DESC depthStencilDesc {};
    depthStencilDesc.DepthEnable = true;
    depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
    depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

    // シェーダの読み込み
    if (!shaderManager_->LoadShader(Line, L"Fragment.VS.hlsl", L"Fragment.PS.hlsl")){
        return;
    }

    // RootSignatureの設定
    D3D12_ROOT_PARAMETER rootParameters[1] = {};

    //wvp/world
    rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
    rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
    rootParameters[0].Descriptor.ShaderRegister = 0;

    D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc = {};
    rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
    rootSignatureDesc.pParameters = rootParameters;
    rootSignatureDesc.NumParameters = _countof(rootParameters);

    // RootSignatureの作成
    ComPtr<ID3DBlob> signatureBlob;
    ComPtr<ID3DBlob> errorBlob;
    HRESULT hr = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
    if (FAILED(hr)){
        if (errorBlob){
            OutputDebugStringA(( char* ) errorBlob->GetBufferPointer());
        }
        return;
    }

    ComPtr<ID3D12RootSignature> rootSignature;
    ComPtr<ID3D12Device> device = dxCore_->GetDevice();
    hr = device->CreateRootSignature(0, signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature));
    if (FAILED(hr)){
        return;
    }

    // PSOの設定
    D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
    psoDesc.pRootSignature = rootSignature.Get();
    psoDesc.InputLayout = inputLayoutDesc;
    psoDesc.VS = {shaderManager_->GetVertexShader(Line)->GetBufferPointer(), shaderManager_->GetVertexShader(Line)->GetBufferSize()};
    psoDesc.PS = {shaderManager_->GetPixelShader(Line)->GetBufferPointer(), shaderManager_->GetPixelShader(Line)->GetBufferSize()};
    psoDesc.BlendState = blendDesc;
    psoDesc.RasterizerState = rasterizeDesc;
    psoDesc.DepthStencilState = depthStencilDesc;
    psoDesc.NumRenderTargets = 1;
    psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
    psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE; // 線描画用に設定
    psoDesc.SampleDesc.Count = 1;
    psoDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
    psoDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

    // パイプラインステートオブジェクトの作成
    if (!pipelineStateManager_->CreatePipelineState(Line, L"Fragment.VS.hlsl", L"Fragment.PS.hlsl", rootSignatureDesc, psoDesc)){
        return;
    }
}
