#include "Model.h"

#include "engine/objects/ModelData.h"
#include "engine/graphics/Material.h"
#include "engine/objects/TransformationMatrix.h"
#include "engine/objects/Transform.h"
#include "lib/myMath/Vector4.h"

#include "engine/graphics/GraphicsGroup.h"
#include "engine/objects/TextureManager.h"
#include "engine/objects/ModelManager.h"
#include "engine/graphics/VertexData.h"
#include "engine/physics/DirectionalLight.h"
#include "Engine/graphics/camera/CameraManager.h"

#ifdef _DEBUG
#include "externals/imgui/imgui.h"
#endif

#include "lib/myfunc/MyFunc.h" // 必要に応じて

Model::Model(const std::string& fileName){
    Create(fileName);
}

Model::~Model(){
    // 必要があれば後処理
}

void Model::Initialize(){
    // Device, CommandListの取得
    device_ = GraphicsGroup::GetInstance()->GetDevice();
    commandList_ = GraphicsGroup::GetInstance()->GetCommandList();

    // ルートシグネチャ・パイプラインを設定(例: Object3D)
    rootSignature_ = GraphicsGroup::GetInstance()->GetRootSignature(Object3D);
    pipelineState_ = GraphicsGroup::GetInstance()->GetPipelineState(Object3D);

    // デフォルト値
    RGBa = {1.0f, 1.0f, 1.0f, 1.0f};
    transform = {{1.0f, 1.0f, 1.0f},
                   {0.0f, 0.0f, 0.0f},
                   {0.0f, 0.0f, 0.0f}};

    materialParameter_.shininess = 20.0f;

    // マテリアル・行列バッファ生成
    CreateMaterialBuffer();
    CreateMatrixBuffer();
    Map();
}

void Model::Create(const std::string& filename){
    // まずは初期化
    Initialize();

    // モデルデータのロード(既に存在すればキャッシュから取得)
    modelData_ = ModelManager::LoadModel(filename);

    // 頂点バッファとインデックスバッファを取得してビューを作成
    auto vertexResource = ModelManager::GetVertexResource(filename);
    auto indexResource = ModelManager::GetIndexResource(filename);

    vertexBufferView_.BufferLocation = vertexResource->GetGPUVirtualAddress();
    vertexBufferView_.SizeInBytes = static_cast< UINT >(sizeof(VertexData) * modelData_->vertices.size());
    vertexBufferView_.StrideInBytes = sizeof(VertexData);

    indexBufferView_.BufferLocation = indexResource->GetGPUVirtualAddress();
    indexBufferView_.SizeInBytes = static_cast< UINT >(sizeof(uint32_t) * modelData_->indices.size());
    indexBufferView_.Format = DXGI_FORMAT_R32_UINT;

    // テクスチャを読み込む
    handle_ = TextureManager::GetInstance()->LoadTexture(modelData_->material.textureFilePath);
}

void Model::Update(){
    // UV transform を行列化 (例: スケール→Z回転→平行移動)
    Matrix4x4 uvTransformMatrix = MakeScaleMatrix(uvTransform.scale);
    uvTransformMatrix = Matrix4x4::Multiply(uvTransformMatrix, MakeRotateZMatrix(uvTransform.rotate.z));
    uvTransformMatrix = Matrix4x4::Multiply(uvTransformMatrix, MakeTranslateMatrix(uvTransform.translate));
    materialData_->uvTransform = uvTransformMatrix;

    // マテリアルの更新
    materialData_->color = RGBa;
    materialData_->shininess = materialParameter_.shininess;
    materialData_->enableLighting = materialParameter_.enableLighting;

    // ワールド行列の更新
    worldMatrix = MakeAffineMatrix(transform.scale, transform.rotate, transform.translate);

    // カメラ行列との掛け合わせ
    Matrix4x4 worldViewProjectionMatrix = Matrix4x4::Multiply(worldMatrix, CameraManager::GetViewProjectionMatrix());
    matrixData_->world = worldMatrix;
    matrixData_->WVP = worldViewProjectionMatrix;
}

void Model::UpdateMatrix(){
    // もし外部から行列のみを更新したい場合などに呼ばれる
    Matrix4x4 worldViewProjectionMatrix = Matrix4x4::Multiply(worldMatrix, CameraManager::GetViewProjectionMatrix());
    matrixData_->world = worldMatrix;
    matrixData_->WVP = worldViewProjectionMatrix;
}

void Model::Map(){
    // マテリアルと行列のマッピング
    MaterialBufferMap();
    MatrixBufferMap();
}

void Model::ShowImGuiInterface(){
    // UV Transform用の行列を再計算(念のため)
    Matrix4x4 uvTransformMatrix = MakeScaleMatrix(uvTransform.scale);
    uvTransformMatrix = Matrix4x4::Multiply(uvTransformMatrix, MakeRotateZMatrix(uvTransform.rotate.z));
    uvTransformMatrix = Matrix4x4::Multiply(uvTransformMatrix, MakeTranslateMatrix(uvTransform.translate));
    materialData_->uvTransform = uvTransformMatrix;

#ifdef _DEBUG
    // ImGui で編集
    if (ImGui::DragFloat3("Translation", &transform.translate.x, 0.01f)){}
    if (ImGui::DragFloat3("Rotation", &transform.rotate.x, 0.01f)){}
    if (ImGui::DragFloat3("Scale", &transform.scale.x, 0.01f)){}
    if (ImGui::DragFloat3("uvScale", &uvTransform.scale.x, 0.01f)){}
    ImGui::DragFloat("shininess", &materialData_->shininess, 0.01f);

    const char* lightingModes[] = {"Half-Lambert", "Lambert", "SpecularReflection", "No Lighting"};
    if (ImGui::BeginCombo("Lighting Mode", lightingModes[currentLightingMode_])){
        for (int n = 0; n < IM_ARRAYSIZE(lightingModes); n++){
            bool is_selected = (currentLightingMode_ == n);
            if (ImGui::Selectable(lightingModes[n], is_selected)){
                currentLightingMode_ = n;
                materialData_->enableLighting = currentLightingMode_;
            }
            if (is_selected){
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }
#endif
}

void Model::Draw(){
    // ルートシグネチャ・パイプラインをセット
    commandList_->SetGraphicsRootSignature(rootSignature_.Get());
    commandList_->SetPipelineState(pipelineState_.Get());

    // 頂点バッファ/インデックスバッファをセット
    commandList_->IASetVertexBuffers(0, 1, &vertexBufferView_);
    commandList_->IASetIndexBuffer(&indexBufferView_);
    commandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // マテリアル & 行列バッファをセット
    commandList_->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
    commandList_->SetGraphicsRootConstantBufferView(1, wvpResource_->GetGPUVirtualAddress());
    commandList_->SetGraphicsRootDescriptorTable(3, handle_);

    // 描画
    commandList_->DrawIndexedInstanced(UINT(modelData_->indices.size()), 1, 0, 0, 0);
}

//==============================================================================
// バッファ生成/マッピング
//==============================================================================
void Model::CreateMaterialBuffer(){
    materialResource_ = CreateBufferResource(device_.Get(), sizeof(Material));
}

void Model::CreateMatrixBuffer(){
    wvpResource_ = CreateBufferResource(device_.Get(), sizeof(TransformationMatrix));
}

void Model::MaterialBufferMap(){
    materialResource_->Map(0, nullptr, reinterpret_cast< void** >(&materialData_));
    materialData_->color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
    materialData_->enableLighting = HalfLambert;  // デフォルト
    materialData_->uvTransform = Matrix4x4::MakeIdentity();
    materialData_->shininess = 20.0f;
    materialResource_->Unmap(0, nullptr);
}

void Model::MatrixBufferMap(){
    wvpResource_->Map(0, nullptr, reinterpret_cast< void** >(&matrixData_));
    matrixData_->WVP = Matrix4x4::MakeIdentity();
    matrixData_->world = Matrix4x4::MakeIdentity();
    wvpResource_->Unmap(0, nullptr);
}
