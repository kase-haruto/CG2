#include "Sprite.h"
#include "DirectXCommon.h"
#include "myfunc/MyFunc.h"
#include "VertexData.h"
#include "imgui.h"
#include "TextureManager.h"
#include "TransformationMatrix.h"
#include "GraphicsGroup.h"
#include <stdint.h>
#include "DirectionalLight.h"

Sprite::Sprite(const std::string& filePath){
    // パイプラインとルートシグネチャの設定
    rootSignature_ = GraphicsGroup::GetInstance()->GetRootSignature(Object2D);
    pipelineState_ = GraphicsGroup::GetInstance()->GetPipelineState(Object2D);

    handle = TextureManager::GetInstance()->LoadTexture(filePath);


    transform_ = {{1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f}};

    path = filePath;

    // リソースの生成
    CreateBuffer();

    // マップ
    Map();

    // トランスフォームと行列の初期化
    UpdateMatrix();
    UpdateTransform();
}

Sprite::~Sprite(){}

void Sprite::Initialize(const Vector2& position, const Vector2& size){
    this->position = position;
    transform_.translate.x = position.x;
    transform_.translate.y = position.y;

    this->size = size;
    transform_.scale.x = size.x;
    transform_.scale.y = size.y;
}

void Sprite::Update(){
//#ifdef DEBUG
//    ImGui::Begin("sprite");
//    ImGui::SliderFloat3("translate", &transform_.translate.x, 0.0f, 1280.0f);
//    ImGui::DragFloat3("rotate", &transform_.rotate.x, 0.01f);
//    ImGui::DragFloat3("scale", &transform_.scale.x, 0.01f);
//    ImGui::DragFloat2("UVTranslate", &uvTransform.translate.x, 0.01f, -10.0f, 10.0f);
//    ImGui::DragFloat2("UVScale", &uvTransform.scale.x, 0.01f, -10.0f, 10.0f);
//    ImGui::End();
//#endif // DEBUG

    

    transform_.translate = {position.x, position.y, 0.0f};
    transform_.rotate = {0.0f, 0.0f, rotate};
    transform_.scale = {size.x, size.y, 1.0f};

    // アンカーポイントの反映
    float left = 0.0f - anchorPoint.x;
    float right = 1.0f - anchorPoint.x;
    float top = 0.0f - anchorPoint.y;
    float bottom = 1.0f - anchorPoint.y;

    vertexData[0].position = {left, bottom, 0.0f, 1.0f};   // 左下
    vertexData[1].position = {left, top, 0.0f, 1.0f};      // 左上
    vertexData[2].position = {right, bottom, 0.0f, 1.0f};  // 右下
    vertexData[3].position = {right, top, 0.0f, 1.0f};     // 右上

    //const DirectX::TexMetadata& metadata = TextureManager::GetInstance()->GetMetaData(path);


    UpdateMatrix();
    UpdateTransform();
}

void Sprite::UpdateMatrix(){
    Matrix4x4 matWorld = MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate);
    Matrix4x4 matView = Matrix4x4::MakeIdentity();
    Matrix4x4 matProjection = MakeOrthographicMatrix(0.0f, 0.0f, 1280.0f, 720.0f, 0.0f, 100.0f);
    Matrix4x4 wvpMatrix = Matrix4x4::Multiply(matWorld, Matrix4x4::Multiply(matView, matProjection));
    *transformData = wvpMatrix;
}

void Sprite::UpdateTransform(){
    ///===================================================
    /// UV Transform
    ///===================================================
    Matrix4x4 uvTransformMatrix = MakeScaleMatrix(uvTransform.scale);
    uvTransformMatrix = Matrix4x4::Multiply(uvTransformMatrix, MakeRotateZMatrix(uvTransform.rotate.z));
    uvTransformMatrix = Matrix4x4::Multiply(uvTransformMatrix, MakeTranslateMatrix(uvTransform.translate));
    materialData_->uvTransform = uvTransformMatrix;
}

void Sprite::Draw(){
    if (!pipelineState_ || !rootSignature_){
        OutputDebugStringA("パイプラインステートまたはルートシグネチャが設定されていません");
        return;
    }

    if (vertexBufferViewSprite.BufferLocation == 0){
        OutputDebugStringA("頂点バッファの GPU アドレスが無効です");
        return;
    }

    auto commandList = GraphicsGroup::GetInstance()->GetCommandList();

    commandList->SetPipelineState(pipelineState_.Get());
    commandList->SetGraphicsRootSignature(rootSignature_.Get());

    commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    commandList->IASetIndexBuffer(&indexBufferView);
    commandList->IASetVertexBuffers(0, 1, &vertexBufferViewSprite);
    commandList->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
    commandList->SetGraphicsRootConstantBufferView(1, transformResource_->GetGPUVirtualAddress());
    commandList->SetGraphicsRootDescriptorTable(2, handle);

    commandList->DrawIndexedInstanced(6, 1, 0, 0, 0);
}

void Sprite::CreateBuffer(){
    auto device = GraphicsGroup::GetInstance()->GetDevice();

    // 頂点用リソース
    vertexResource_ = CreateBufferResource(device, sizeof(VertexData) * 4);

    vertexBufferViewSprite.BufferLocation = vertexResource_->GetGPUVirtualAddress();
    vertexBufferViewSprite.SizeInBytes = static_cast < UINT >(sizeof(VertexData)) * 4;
    vertexBufferViewSprite.StrideInBytes = static_cast < UINT >(sizeof(VertexData));

    // トランスフォーム用リソース
    transformResource_ = CreateBufferResource(device, sizeof(TransformationMatrix));

    // マテリアル用リソース
    materialResource_ = CreateBufferResource(device, sizeof(Material2D));

    // インデックス用リソース
    indexResource_ = CreateBufferResource(device, sizeof(uint32_t) * 6);
    indexBufferView.BufferLocation = indexResource_->GetGPUVirtualAddress();
    indexBufferView.SizeInBytes = static_cast < UINT >(sizeof(uint32_t)) * 6;
    indexBufferView.Format = DXGI_FORMAT_R32_UINT;
}

void Sprite::Map(){
    IndexResourceMap();
    VertexResourceMap();
    TransformResourceMap();
    MaterialResourceMap();
}

void Sprite::IndexResourceMap(){
    uint32_t* indexData = nullptr;
    indexResource_->Map(0, nullptr, reinterpret_cast< void** >(&indexData));

    indexData[0] = 0; indexData[1] = 1; indexData[2] = 2;
    indexData[3] = 1; indexData[4] = 3; indexData[5] = 2;

    indexResource_->Unmap(0, nullptr);
}

void Sprite::VertexResourceMap(){
    vertexResource_->Map(0, nullptr, reinterpret_cast< void** >(&vertexData));

    // UV座標の設定
    vertexData[0].texcoord = {0.0f, 1.0f}; // 左下
    vertexData[1].texcoord = {0.0f, 0.0f}; // 左上
    vertexData[2].texcoord = {1.0f, 1.0f}; // 右下
    vertexData[3].texcoord = {1.0f, 0.0f}; // 右上

    // アンカーポイントの反映
    float left = 0.0f - anchorPoint.x;
    float right = 1.0f - anchorPoint.x;
    float top = 0.0f - anchorPoint.y;
    float bottom = 1.0f - anchorPoint.y;

    vertexData[0].position = {left, bottom, 0.0f, 1.0f};   // 左下
    vertexData[1].position = {left, top, 0.0f, 1.0f};      // 左上
    vertexData[2].position = {right, bottom, 0.0f, 1.0f};  // 右下
    vertexData[3].position = {right, top, 0.0f, 1.0f};     // 右上

    vertexResource_->Unmap(0, nullptr);
}

void Sprite::TransformResourceMap(){
    transformResource_->Map(0, nullptr, reinterpret_cast< void** >(&transformData));
    *transformData = Matrix4x4::MakeIdentity();  // 初期値は単位行列
}

void Sprite::MaterialResourceMap(){
    materialResource_->Map(0, nullptr, reinterpret_cast< void** >(&materialData_));
    materialData_->color = {1.0f, 1.0f, 1.0f, 1.0f};
    materialData_->uvTransform = Matrix4x4::MakeIdentity();
}

void Sprite::SetTextureHandle(D3D12_GPU_DESCRIPTOR_HANDLE newHandle){
    handle = newHandle;
}

