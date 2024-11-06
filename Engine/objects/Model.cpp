#include "Model.h"
#include "myfunc/MyFunc.h"
#include "VertexData.h"
#include "TextureManager.h"
#include "objects/ModelManager.h"
#include "GraphicsGroup.h"
#include "DirectionalLight.h"
#ifdef _DEBUG
#include "imgui.h"
#endif // _DEBUG

Model::Model(const std::string& fileName){
    Create(fileName);

}

Model::~Model(){}

void Model::Initialize(bool isUseTexture){
    device_ = GraphicsGroup::GetInstance()->GetDevice();
    commandList_ = GraphicsGroup::GetInstance()->GetCommandList();

    // パイプラインを設定
    rootSignature_ = GraphicsGroup::GetInstance()->GetRootSignature(Object3D);
    pipelineState_ = GraphicsGroup::GetInstance()->GetPipelineState(Object3D);

    RGBa = {1.0f, 1.0f, 1.0f, 1.0f};
    transform = {{1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}};

    // マテリアルと行列用リソースを生成
    CreateMaterialBuffer();
    CreateMatrixBuffer();
    Map();

}

void Model::ShowImGuiInterface(){
    Matrix4x4 uvTransformMatrix = MakeScaleMatrix(uvTransform.scale);
    uvTransformMatrix = Matrix4x4::Multiply(uvTransformMatrix, MakeRotateZMatrix(uvTransform.rotate.z));
    uvTransformMatrix = Matrix4x4::Multiply(uvTransformMatrix, MakeTranslateMatrix(uvTransform.translate));
    materialData->uvTransform = uvTransformMatrix;

#ifdef _DEBUG


    ImGui::DragFloat3("Translation", &transform.translate.x, 0.01f);
    ImGui::DragFloat3("Rotation", &transform.rotate.x, 0.01f);
    ImGui::DragFloat3("size", &transform.scale.x, 0.01f);
    ImGui::DragFloat3("uvScale", &uvTransform.scale.x, 0.01f);
    ImGui::DragFloat("shininess", &materialData->shininess, 0.01f);


    const char* lightingModes[] = {"Half-Lambert", "Lambert","SpecularReflection", "No Lighting"};

    if (ImGui::BeginCombo("Lighting Mode", lightingModes[currentLightingMode])){
        for (int n = 0; n < IM_ARRAYSIZE(lightingModes); n++){
            const bool is_selected = (currentLightingMode == n);
            if (ImGui::Selectable(lightingModes[n], is_selected)){
                currentLightingMode = n;
                materialData->enableLighting = currentLightingMode;
            }
            if (is_selected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }
#endif // DEBUG

}

void Model::Create(const std::string& filename, bool isUseTexture){
    Initialize();

    // モデルの読み込み（既に存在すればロードしない）
    modelData = ModelManager::LoadModel(filename);

    // 頂点バッファとインデックスバッファを取得してビューを作成
    auto vertexResource = ModelManager::GetVertexResource(filename);
    auto indexResource = ModelManager::GetIndexResource(filename);

    vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();
    vertexBufferView.SizeInBytes = static_cast< UINT >(sizeof(VertexData) * modelData->vertices.size());
    vertexBufferView.StrideInBytes = sizeof(VertexData);

    indexBufferView.BufferLocation = indexResource->GetGPUVirtualAddress();
    indexBufferView.SizeInBytes = static_cast< UINT >(sizeof(uint32_t) * modelData->indices.size());
    indexBufferView.Format = DXGI_FORMAT_R32_UINT;

    handle = TextureManager::GetInstance()->LoadTexture(modelData->material.textureFilePath);
}

void Model::Update(){
    materialData->color = Vector4(RGBa.x, RGBa.y, RGBa.z, RGBa.w);

    worldMatrix = MakeAffineMatrix(transform.scale, transform.rotate, transform.translate);
    Matrix4x4 worldViewProjectionMatrix = Matrix4x4::Multiply(worldMatrix, viewProjection_->GetViewProjection());
    matrixData->world = worldMatrix;
    matrixData->WVP = worldViewProjectionMatrix;
}

void Model::UpdateMatrix(){
    Matrix4x4 worldViewProjectionMatrix = Matrix4x4::Multiply(worldMatrix, viewProjection_->GetViewProjection());
    matrixData->world = worldMatrix;
    matrixData->WVP = worldViewProjectionMatrix;
}

void Model::Draw(){

    commandList_->SetGraphicsRootSignature(rootSignature_.Get());
    commandList_->SetPipelineState(pipelineState_.Get());

    // 頂点バッファとインデックスバッファを設定
    commandList_->IASetVertexBuffers(0, 1, &vertexBufferView);
    commandList_->IASetIndexBuffer(&indexBufferView);
    commandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // マテリアルと行列バッファを設定
    commandList_->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
    commandList_->SetGraphicsRootConstantBufferView(1, wvpResource_->GetGPUVirtualAddress());
    commandList_->SetGraphicsRootDescriptorTable(3, handle);

    // インデックス付き描画
    commandList_->DrawIndexedInstanced(UINT(modelData->indices.size()), 1, 0, 0, 0);
}

void Model::CreateMaterialBuffer(){
    materialResource_ = CreateBufferResource(device_.Get(), sizeof(Material));
}

void Model::CreateMatrixBuffer(){
    wvpResource_ = CreateBufferResource(device_.Get(), sizeof(TransformationMatrix));
}

void Model::Map(){
    // マテリアルと行列リソースにデータを書き込む
    MaterialBufferMap();
    MatrixBufferMap();
}

void Model::MaterialBufferMap(){
    materialResource_->Map(0, nullptr, reinterpret_cast< void** >(&materialData));
    materialData->color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
    materialData->enableLighting = HalfLambert;
    materialData->uvTransform = Matrix4x4::MakeIdentity();
    materialData->shininess = 20.0f;
    materialResource_->Unmap(0, nullptr);
}

void Model::MatrixBufferMap(){
    wvpResource_->Map(0, nullptr, reinterpret_cast< void** >(&matrixData));
    matrixData->WVP = Matrix4x4::MakeIdentity();
    wvpResource_->Unmap(0, nullptr);
}

void Model::SetViewProjection(const ViewProjection* viewPro){
    viewProjection_ = viewPro;
}
