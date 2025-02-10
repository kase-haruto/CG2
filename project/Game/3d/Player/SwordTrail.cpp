#include "SwordTrail.h"

#include "engine/graphics/GraphicsGroup.h"
#include "engine/objects/TextureManager.h"
#include "Engine/graphics/camera/CameraManager.h"
#include "Engine/core/System.h"


#include "lib/myFunc/MyFunc.h" // MakeAffineMatrix, etc
#include <algorithm>
#undef max

//----------------------------------------------------------------------
 // Initialize
//----------------------------------------------------------------------
void SwordTrail::Initialize(){
    // テクスチャを読み込む (例: trail.png)
    textureHandle_ = TextureManager::GetInstance()->LoadTexture("trail.png");

    // フェード設定
    fadeSpeed_ = 4.0f;
    minAlpha_ = 0.05f;

    CreateBuffer();
    Map();
}

//----------------------------------------------------------------------
 // Create Buffer Resources
//----------------------------------------------------------------------
//----------------------------------------------------------------------
// Create Buffer Resources
//----------------------------------------------------------------------
void SwordTrail::CreateVertexBuffer(Microsoft::WRL::ComPtr<ID3D12Device> device){
    const UINT vbSize = static_cast< UINT >(sizeof(EffectVertexData) * MAX_TRAIL_VERTICES);
    vertexResource_ = CreateBufferResource(device.Get(), vbSize);

    vbView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
    vbView_.StrideInBytes = sizeof(EffectVertexData);
    vbView_.SizeInBytes = vbSize;
}

void SwordTrail::CreateMaterialBuffer(Microsoft::WRL::ComPtr<ID3D12Device> device){
    materialResource_ = CreateBufferResource(device.Get(), sizeof(Material));
}

void SwordTrail::CreateMatrixBuffer(Microsoft::WRL::ComPtr<ID3D12Device> device){
    matrixResource_ = CreateBufferResource(device.Get(), sizeof(TransformationMatrix));
}

void SwordTrail::CreateBuffer(){
    Microsoft::WRL::ComPtr<ID3D12Device> device = GraphicsGroup::GetInstance()->GetDevice();
    CreateVertexBuffer(device);
    CreateMaterialBuffer(device);
    CreateMatrixBuffer(device);

    if (!vertexResource_){
        assert(false);
    }
}

//----------------------------------------------------------------------
 // Map
//----------------------------------------------------------------------
void SwordTrail::Map(){
    VertexBufferMap();
    MaterialBufferMap();
    MatrixBufferMap();
}

void SwordTrail::VertexBufferMap(){
    vertexResource_->Map(0, nullptr, reinterpret_cast< void** >(&mappedVertices_));
}

void SwordTrail::MaterialBufferMap(){
    materialResource_->Map(0, nullptr, reinterpret_cast< void** >(&materialData_));
    materialData_->color = Vector4(1, 1, 1, 1);
    materialData_->enableLighting = 0;
    materialData_->uvTransform = Matrix4x4::MakeIdentity();
    materialData_->shininess = 20.0f;
    materialResource_->Unmap(0, nullptr);
}

void SwordTrail::MatrixBufferMap(){
    matrixResource_->Map(0, nullptr, reinterpret_cast< void** >(&matrixData_));
    matrixData_->world = Matrix4x4::MakeIdentity();
    matrixData_->WVP = Matrix4x4::MakeIdentity();
    matrixResource_->Unmap(0, nullptr);
}

//----------------------------------------------------------------------
 // AddSegment (tip/base 2頂点を追加)
//----------------------------------------------------------------------
void SwordTrail::AddSegment(const Vector3& tip, const Vector3& base){
    // 容量オーバー時の頂点削除
    if (vertices_.size() + 2 > MAX_TRAIL_VERTICES){
        if (!vertices_.empty()){
            vertices_.erase(vertices_.begin(), vertices_.begin() + 2);
        }
    }

    // 先端の移動距離を使って U 座標を計算
    if (segmentCount_ == 0){
        lastTip_ = tip;
        accumulatedLength_ = 0.0f;
    } else{
        float dist = (tip - lastTip_).Length();
        accumulatedLength_ += dist;
        lastTip_ = tip;
    }

    // テクスチャの U 座標を計算
    float textureScale = 1.0f;
    float uCoord = std::fmod(accumulatedLength_ * textureScale, 1.0f);

    // セグメントカウントをインクリメント
    ++segmentCount_;

    // **アルファの設定**
    float alphaRoot = 0.2f; // 根元のアルファを低めに設定 (0.0 ~ 1.0 の範囲)

    // 先端 (Alpha = 1.0)
    {
        EffectVertexData v;
        v.position = {tip.x, tip.y, tip.z, 1.0f};
        v.texcoord = {uCoord, 0.0f};
        v.color = {1.0f, 1.0f, 1.0f, 1.0f};  // 先端は完全不透明

        vertices_.push_back(v);
    }

    // 根元 (Alpha = alphaRoot)
    {
        EffectVertexData v;
        v.position = {base.x, base.y, base.z, 1.0f};
        v.texcoord = {uCoord, 1.0f};
        v.color = {1.0f, 1.0f, 1.0f, alphaRoot};  // 根元は薄く

        vertices_.push_back(v);
    }
}

//----------------------------------------------------------------------
 // Update (アルファフェード → 2頂点単位の削除)
//----------------------------------------------------------------------
void SwordTrail::Update(float deltaTime){
    // 全ての頂点のアルファ値を減少させる
    for (auto& vertex : vertices_){
        vertex.color.w -= fadeSpeed_ * deltaTime;
        if (vertex.color.w < 0.0f){
            vertex.color.w = 0.0f;
        }
    }

    // 先頭から2頂点ペア単位でチェックし、alpha < minAlpha_ なら削除
    for (size_t i = 0; i + 1 < vertices_.size(); /* i += 2 */){
        float alpha0 = vertices_[i].color.w;
        float alpha1 = vertices_[i + 1].color.w;

        // 2頂点とも minAlpha_ 以下なら削除
        if (alpha0 < minAlpha_ && alpha1 < minAlpha_){
            vertices_.erase(vertices_.begin() + i, vertices_.begin() + i + 2);
        } else{
            i += 2; // 次のペアへ
        }
    }

    // 行列更新 & 頂点バッファ更新
    UpdateMatrix();
    UpdateVertexBuffer();
}

//----------------------------------------------------------------------
 // UpdateMatrix
//----------------------------------------------------------------------
void SwordTrail::UpdateMatrix(){
    Matrix4x4 world = Matrix4x4::MakeIdentity();
    Matrix4x4 wvp = Matrix4x4::Multiply(world, CameraManager::GetViewProjectionMatrix());

    // CBV に書き込み
    matrixResource_->Map(0, nullptr, reinterpret_cast< void** >(&matrixData_));
    matrixData_->world = world;
    matrixData_->WVP = wvp;
    matrixResource_->Unmap(0, nullptr);
}

//----------------------------------------------------------------------
 // UpdateVertexBuffer (CPU→GPU)
//----------------------------------------------------------------------
void SwordTrail::UpdateVertexBuffer(){
    // vertices_ を mappedVertices_ にコピー
    for (size_t i = 0; i < vertices_.size(); ++i){
        mappedVertices_[i] = vertices_[i];
    }
}

//----------------------------------------------------------------------
 // Draw
//----------------------------------------------------------------------
void SwordTrail::Draw(){
    // 4頂点未満の場合はストリップで三角形を形成できない
    if (vertices_.size() < 4){
        return;
    }

    ComPtr<ID3D12PipelineState> pso = GraphicsGroup::GetInstance()->GetPipelineState(PipelineType::Effect, BlendMode::NORMAL);
    ComPtr<ID3D12RootSignature> rootSig = GraphicsGroup::GetInstance()->GetRootSignature(PipelineType::Effect, BlendMode::NORMAL);
    ComPtr<ID3D12GraphicsCommandList> commandList = GraphicsGroup::GetInstance()->GetCommandList();
    // ルートシグネチャ & パイプライン設定
    commandList->SetGraphicsRootSignature(rootSig.Get());
    commandList->SetPipelineState(pso.Get());
    // ==== TriangleStrip を指定 ====
    commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

    // 頂点バッファ設定
    commandList->IASetVertexBuffers(0, 1, &vbView_);

    // CBV (Material & Matrix)
    commandList->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
    commandList->SetGraphicsRootConstantBufferView(1, matrixResource_->GetGPUVirtualAddress());

    // SRV (テクスチャ)
    commandList->SetGraphicsRootDescriptorTable(2, textureHandle_);

    // Draw (頂点数だけストリップを進める)
    commandList->DrawInstanced(
        static_cast< UINT >(vertices_.size()),  // VertexCountPerInstance
        1,                                   // InstanceCount
        0,                                   // StartVertexLocation
        0                                    // StartInstanceLocation
    );
}

//----------------------------------------------------------------------
 // Clear
//----------------------------------------------------------------------
void SwordTrail::Clear(){
    vertices_.clear();
    segmentCount_ = 0;
}
