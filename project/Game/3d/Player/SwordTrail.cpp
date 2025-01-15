#include "SwordTrail.h"

#include "engine/graphics/GraphicsGroup.h"
#include "engine/objects/TextureManager.h"
#include "Engine/graphics/camera/CameraManager.h"
#include "engine/physics/DirectionalLight.h"
#include "Engine/core/System.h"

#include "lib/myFunc/MyFunc.h" // MakeAffineMatrix, etc
#include <algorithm>
#undef max

//----------------------------------------------------------------------
// Initialize
//----------------------------------------------------------------------
void SwordTrail::Initialize(
    Microsoft::WRL::ComPtr<ID3D12Device> device,
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> cmdList,
    Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSig,
    Microsoft::WRL::ComPtr<ID3D12PipelineState> pso){
    device_ = device;
    commandList_ = cmdList;
    rootSignature_ = rootSig;
    pipelineState_ = pso;

    // テクスチャを読み込む (例: white1x1.png)
    textureHandle_ = TextureManager::GetInstance()->LoadTexture("trail.png");

    CreateVertexBuffer();
    CreateMaterialBuffer();
    CreateMatrixBuffer();
    Map();
}

//----------------------------------------------------------------------
// Create Buffer Resources
//----------------------------------------------------------------------
void SwordTrail::CreateVertexBuffer(){
    const UINT vbSize = static_cast< UINT >(sizeof(VertexData) * MAX_TRAIL_VERTICES);
    vertexResource_ = CreateBufferResource(device_.Get(), vbSize);

    vbView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
    vbView_.StrideInBytes = sizeof(VertexData);
    vbView_.SizeInBytes = vbSize;
}

void SwordTrail::CreateMaterialBuffer(){
    materialResource_ = CreateBufferResource(device_.Get(), sizeof(Material));
}

void SwordTrail::CreateMatrixBuffer(){
    matrixResource_ = CreateBufferResource(device_.Get(), sizeof(TransformationMatrix));
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
    materialData_->enableLighting = HalfLambert;
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
    // 容量オーバー時に古い頂点を先に削除
    // (2頂点追加できないなら先頭から削除 etc...)
    if (vertices_.size() + 2 > MAX_TRAIL_VERTICES){
        // とりあえず、先頭から2頂点削除 (1セグメントぶん)
        if (!vertices_.empty()){
            vertices_.erase(vertices_.begin(), vertices_.begin() + 2);
        }
    }

    // 今回のセグメントの u 座標を計算
    float uCoord = static_cast< float >(segmentCount_++) * uvStep_;

    // 先端
    {
        VertexData v;
        v.position = {tip.x,  tip.y,  tip.z, 1.0f};    // w に alpha=1.0
        v.texcoord = {uCoord, 0.0f};
        v.normal = {0.0f, 0.0f, -1.0f};             // 仮の法線

        vertices_.push_back(v);
    }

    // 根元
    {
        VertexData v;
        v.position = {base.x, base.y, base.z, 1.0f};
        v.texcoord = {uCoord, 1.0f};
        v.normal = {0.0f, 0.0f, -1.0f};

        vertices_.push_back(v);
    }
}

//----------------------------------------------------------------------
// Update (アルファフェード → 2頂点単位の削除)
//----------------------------------------------------------------------
void SwordTrail::Update([[maybe_unused]]float deltaTime){
    // 先頭から2頂点ペア単位でチェックし、alpha < minAlpha_ なら削除
    // ※ 三角形ストリップの連続性を保つため 2頂点ずつ削除する
    // 例: (tip0, base0), (tip1, base1), ...



    for (size_t i = 0; i + 1 < vertices_.size(); /* i += 2 は後で */){
        float alpha0 = vertices_[i].position.w;
        float alpha1 = vertices_[i + 1].position.w;

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

    // ルートシグネチャ & パイプライン設定
    commandList_->SetGraphicsRootSignature(rootSignature_.Get());
    commandList_->SetPipelineState(pipelineState_.Get());

    // 頂点バッファ設定
    commandList_->IASetVertexBuffers(0, 1, &vbView_);

    // ==== TriangleStrip を指定 ====
    commandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

    // CBV (Material & Matrix)
    commandList_->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
    commandList_->SetGraphicsRootConstantBufferView(1, matrixResource_->GetGPUVirtualAddress());

    // SRV (テクスチャ)
    commandList_->SetGraphicsRootDescriptorTable(3, textureHandle_);

    // Draw (頂点数だけストリップを進める)
    commandList_->DrawInstanced(
        static_cast< UINT >(vertices_.size()),  // VertexCountPerInstance
        1,                                    // InstanceCount
        0,                                    // StartVertexLocation
        0                                     // StartInstanceLocation
    );
}

//----------------------------------------------------------------------
// Clear
//----------------------------------------------------------------------
void SwordTrail::Clear(){
    vertices_.clear();
    segmentCount_ = 0;
}