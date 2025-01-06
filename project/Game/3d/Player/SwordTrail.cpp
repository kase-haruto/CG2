#include "SwordTrail.h"
#include "engine/graphics/GraphicsGroup.h"
#include "engine/objects/TextureManager.h"
#include "Engine/graphics/camera/CameraManager.h"
#include "engine/physics/DirectionalLight.h"
#include "Engine/core/System.h"

#include "lib/myFunc/MyFunc.h" // MakeAffineMatrix, etc
#include <algorithm>
#undef max

void SwordTrail::Initialize(
    Microsoft::WRL::ComPtr<ID3D12Device> device,
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> cmdList,
    Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSig,
    Microsoft::WRL::ComPtr<ID3D12PipelineState> pso){
    device_ = device;
    commandList_ = cmdList;
    rootSignature_ = rootSig;
    pipelineState_ = pso;

    // 例: テクスチャ(uvChecker.png)
    textureHandle_ = TextureManager::GetInstance()->LoadTexture("uvChecker.png");

    // バッファ作成 & マップ
    CreateVertexBuffer();
    CreateMaterialBuffer();
    CreateMatrixBuffer();
    Map();
}

void SwordTrail::CreateVertexBuffer(){
    // TrailVertex * MAX_TRAIL_VERTICES
    const UINT vbSize = static_cast< UINT >(sizeof(TrailVertex) * MAX_TRAIL_VERTICES);
    vertexResource_ = CreateBufferResource(device_.Get(), vbSize);

    vbView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
    vbView_.StrideInBytes = sizeof(TrailVertex);
    vbView_.SizeInBytes = vbSize;
}

void SwordTrail::CreateMaterialBuffer(){
    materialResource_ = CreateBufferResource(device_.Get(), sizeof(Material));
}

void SwordTrail::CreateMatrixBuffer(){
    matrixResource_ = CreateBufferResource(device_.Get(), sizeof(TransformationMatrix));
}

void SwordTrail::Map(){
    VertexBufferMap();
    MaterialBufferMap();
    MatrixBufferMap();
}

void SwordTrail::VertexBufferMap(){
    // 永続Mapしておき、頂点追加のたびに書き込めるようにする
    vertexResource_->Map(0, nullptr, reinterpret_cast< void** >(&mappedVertices_));
}

void SwordTrail::MaterialBufferMap(){
    materialResource_->Map(0, nullptr, reinterpret_cast< void** >(&materialData_));
    materialData_->color = Vector4(1, 1, 1, 1);
    materialData_->enableLighting = HalfLambert;
    materialData_->uvTransform = Matrix4x4::MakeIdentity();
    materialData_->shininess = 20.0f;
    materialResource_->Unmap(0, nullptr);
}

void SwordTrail::MatrixBufferMap(){
    matrixResource_->Map(0, nullptr, reinterpret_cast< void** >(&matrixData_));
    // 初期値
    matrixData_->world = Matrix4x4::MakeIdentity();
    matrixData_->WVP = Matrix4x4::MakeIdentity();
    matrixResource_->Unmap(0, nullptr);
}

void SwordTrail::AddSegment(const Vector3& tip, const Vector3& base){
    // 容量オーバー時に先頭を削除
    if (vertices_.size() + 2 > MAX_TRAIL_VERTICES){
        if (!vertices_.empty()){
            vertices_.erase(vertices_.begin(), vertices_.begin() + 2);
        }
    }

    // tip頂点
    TrailVertex vt;
    vt.position = {tip.x, tip.y, tip.z, 1.0f};
    vt.color = {1.0f, 1.0f, 1.0f, 1.0f};
    vt.normal = {0.0f, 0.0f, -1.0f};
    vertices_.push_back(vt);

    // base頂点
    TrailVertex vb;
    vb.position = {base.x, base.y, base.z, 1.0f};
    vb.color = {1.0f, 1.0f, 1.0f, 1.0f};
    vb.normal = {0.0f, 0.0f, -1.0f};
    vertices_.push_back(vb);
}

void SwordTrail::Update(float deltaTime){
    // フェードアウト
    for (auto& v : vertices_){
        v.color.w = std::max(v.color.w - fadeSpeed_ * deltaTime, 0.0f); // 負のalphaを防止
    }

    // 非アクティブ頂点をリストの末尾に移動し、一括削除
    auto it = std::remove_if(vertices_.begin(), vertices_.end(), [this] (const TrailVertex& v){
        return v.color.w < minAlpha_;
                             });
    vertices_.erase(it, vertices_.end());

    // 行列更新 & 頂点バッファ更新
    UpdateMatrix();
    UpdateVertexBuffer();
}

void SwordTrail::UpdateMatrix(){
    // トレイルはワールド座標を直接入れているので、World行列は単位行列
    // カメラのVPを掛けるだけ
    Matrix4x4 world = Matrix4x4::MakeIdentity();
    Matrix4x4 wvp = Matrix4x4::Multiply(world, CameraManager::GetViewProjectionMatrix());

    // 書き込み
    matrixResource_->Map(0, nullptr, reinterpret_cast< void** >(&matrixData_));
    matrixData_->world = world;
    matrixData_->WVP = wvp;
    matrixResource_->Unmap(0, nullptr);
}

void SwordTrail::UpdateVertexBuffer(){
    // vertices_ を mappedVertices_ にコピー
    for (size_t i = 0; i < vertices_.size(); ++i){
        mappedVertices_[i] = vertices_[i];
    }
}

void SwordTrail::Draw(){
    if (vertices_.size() < 2){
        return; // セグメントがない
    }
    
    commandList_->SetGraphicsRootSignature(rootSignature_.Get());
    commandList_->SetPipelineState(pipelineState_.Get());

    // 頂点バッファ設定
    commandList_->IASetVertexBuffers(0, 1, &vbView_);
    commandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // Material & Matrix
    commandList_->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
    commandList_->SetGraphicsRootConstantBufferView(1, matrixResource_->GetGPUVirtualAddress());

    // テクスチャ
    commandList_->SetGraphicsRootDescriptorTable(3, textureHandle_);

    // 描画
    commandList_->DrawInstanced(
        static_cast< UINT >(vertices_.size()), // 頂点数
        1,                                   // InstanceCount
        0,                                   // StartVertexLocation
        0                                    // StartInstanceLocation
    );
}

void SwordTrail::Clear(){
    vertices_.clear();
}
