#include "Particle.h"
#include "DirectXCommon.h"
#include "MyFunc.h"
#include "VertexData.h"
#include "TextureManager.h"
#include "GraphicsGroup.h"
#include "DirectionalLight.h"
#include "SrvLocator.h"

#ifdef _DEBUG
#include "imgui.h"
#endif // _DEBUG

#include<string>

Particle::Particle(){}

Particle::~Particle(){
}

void Particle::Initialize(ViewProjection* viewProjection){
    device_ = GraphicsGroup::GetInstance()->GetDevice();
    commandList_ = GraphicsGroup::GetInstance()->GetCommandList();
    rootSignature_ = GraphicsGroup::GetInstance()->GetRootSignature(StructuredObject);
    pipelineState_ = GraphicsGroup::GetInstance()->GetPipelineState(StructuredObject);
    viewProjection_ = viewProjection;

    modelData = LoadObjFile("Resources", "plane.obj");
    RGBa = {1.0f, 1.0f, 1.0f, 1.0f};

    for (uint32_t index = 0; index < kNumInstance; ++index){
        transforms[index].scale = {1.0f, 1.0f, 1.0f};
        transforms[index].rotate = {0.0f, 0.0f, 0.0f};
        transforms[index].translate = {index * 0.1f, index * -0.1f, index * 0.1f};
    }

    handle = TextureManager::GetInstance()->LoadTexture(modelData.material.textureFilePath);
    if (!handle.ptr){
        throw std::runtime_error("Failed to load texture.");
    }

    CreateBuffer();
    Map();
    CreateSRV();

  
}

void Particle::CreateSRV(){
    auto [srvHandleCPU, srvHandleGPU] = SrvLocator::AllocateSrv();

    D3D12_SHADER_RESOURCE_VIEW_DESC instancingSrvDesc = {};
    instancingSrvDesc.Format = DXGI_FORMAT_UNKNOWN;
    instancingSrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    instancingSrvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
    instancingSrvDesc.Buffer.FirstElement = 0;
    instancingSrvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
    instancingSrvDesc.Buffer.NumElements = kNumInstance;
    instancingSrvDesc.Buffer.StructureByteStride = sizeof(TransformationMatrix);

    device_->CreateShaderResourceView(instancingResource_.Get(), &instancingSrvDesc, srvHandleCPU);
    instancingSrvHandleGPU_ = srvHandleGPU;
}

void Particle::Update(){
#ifdef _DEBUG
    ImGui::Begin("particle");
    ImGui::ColorEdit4("color", &RGBa.x);
    for (uint32_t index = 0; index < kNumInstance; ++index){
        std::string label = "Transform[" + std::to_string(index) + "]";
        ImGui::DragFloat3(label.c_str(), &transforms[index].translate.x, 0.01f);
    }
    ImGui::End();
#endif // _DEBUG

    materialData->color = RGBa;

    for (uint32_t index = 0; index < kNumInstance; ++index){
        Matrix4x4 worldMatrix = MakeAffineMatrix(transforms[index].scale, transforms[index].rotate, transforms[index].translate);
        Matrix4x4 worldViewProjectionMatrix = Matrix4x4::Multiply(worldMatrix, viewProjection_->GetViewProjection());
        instancingData[index].WVP = worldViewProjectionMatrix;
        instancingData[index].world = worldMatrix;
    }
}

void Particle::Draw(){
    // ルートシグネチャを設定
    commandList_->SetGraphicsRootSignature(rootSignature_.Get());

    // パイプラインステートを設定
    commandList_->SetPipelineState(pipelineState_.Get());

    // 頂点バッファを設定
    commandList_->IASetVertexBuffers(0, 1, &vertexBufferView);

    // プリミティブトポロジーを設定（ここでは三角形リストを指定）
    commandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // マテリアル用の定数バッファをルートパラメータ0にバインド
    commandList_->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());

    // インスタンシング用のSRV（Shader Resource View）をルートパラメータ1にバインド
    commandList_->SetGraphicsRootDescriptorTable(1, instancingSrvHandleGPU_);

    // テクスチャ用のSRVをルートパラメータ2にバインド
    commandList_->SetGraphicsRootDescriptorTable(2, handle);

    // インスタンシングを用いて、指定された頂点バッファの頂点数分だけ描画を行う
    commandList_->DrawInstanced(static_cast< UINT >(modelData.vertices.size()), kNumInstance, 0, 0);

}

void Particle::CreateBuffer(){
    CreateVertexBuffer();
    CreateMaterialBuffer();
    CreateMatrixBuffer();
}

void Particle::CreateVertexBuffer(){
    vertexResource_ = CreateBufferResource(device_.Get(), sizeof(VertexData) * modelData.vertices.size());
    vertexBufferView.BufferLocation = vertexResource_->GetGPUVirtualAddress();
    vertexBufferView.SizeInBytes = static_cast< UINT >(sizeof(VertexData) * modelData.vertices.size());
    vertexBufferView.StrideInBytes = sizeof(VertexData);
}

void Particle::CreateMaterialBuffer(){
    materialResource_ = CreateBufferResource(device_.Get(), sizeof(Material));
}

void Particle::CreateMatrixBuffer(){
    instancingResource_ = CreateBufferResource(device_.Get(), sizeof(TransformationMatrix) * kNumInstance);
}

void Particle::Map(){
    VertexBufferMap();
    MaterialBufferMap();
    MatrixBufferMap();
}

void Particle::VertexBufferMap(){
    VertexData* vertexData = nullptr;
    HRESULT hr = vertexResource_->Map(0, nullptr, reinterpret_cast< void** >(&vertexData));
    if (FAILED(hr)){
        throw std::runtime_error("Failed to map vertex buffer.");
    }
    std::memcpy(vertexData, modelData.vertices.data(), sizeof(VertexData) * modelData.vertices.size());
}

void Particle::MaterialBufferMap(){
    HRESULT hr = materialResource_->Map(0, nullptr, reinterpret_cast< void** >(&materialData));
    if (FAILED(hr)){
        throw std::runtime_error("Failed to map material buffer.");
    }
    materialData->color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
    materialData->enableLighting = Lambert;
    materialData->uvTransform = Matrix4x4::MakeIdentity();
}

void Particle::MatrixBufferMap(){
    HRESULT hr = instancingResource_->Map(0, nullptr, reinterpret_cast< void** >(&instancingData));
    if (FAILED(hr)){
        throw std::runtime_error("Failed to map instance buffer.");
    }
    for (uint32_t index = 0; index < kNumInstance; ++index){
        instancingData[index].WVP = Matrix4x4::MakeIdentity();
        instancingData[index].world = Matrix4x4::MakeIdentity();
    }
}
