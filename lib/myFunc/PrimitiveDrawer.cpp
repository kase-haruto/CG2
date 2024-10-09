#include "myFunc/PrimitiveDrawer.h"
#include "GraphicsGroup.h"
#include "myFunc/MyFunc.h"


PrimitiveDrawer::PrimitiveDrawer(){
    // GraphicsGroup から必要なデバイスとコマンドリストを取得
    device_ = GraphicsGroup::GetInstance()->GetDevice();
    commandList_ = GraphicsGroup::GetInstance()->GetCommandList();
    rootSignature_ = GraphicsGroup::GetInstance()->GetRootSignature(Line);
    pipelineState_ = GraphicsGroup::GetInstance()->GetPipelineState(Line);
}

PrimitiveDrawer::~PrimitiveDrawer(){}

void PrimitiveDrawer::Initialize(){
    // 描画用のメッシュを作成
    CreateMeshes();
    CreateMatrixBuffer();
}

void PrimitiveDrawer::Finalize(){
    if (line_->indexBuff){
        line_->indexBuff->Release();
        line_->indexBuff = nullptr;
    }
    if (line_->vertBuff){
        line_->vertBuff->Release();
        line_->vertBuff = nullptr;
    }

    // wvpResourceの解放
    if (wvpResource_){
        wvpResource_->Release();
        wvpResource_ = nullptr;
    }

    device_.Reset();
    commandList_.Reset();
    pipelineState_.Reset();
    rootSignature_.Reset();

}

void PrimitiveDrawer::CreateMeshes(){
    // 最大の線の数を想定してメッシュを作成
    const UINT maxVertices = kMaxLineCount * kVertexCountLine; // ラインは2頂点で構成
    const UINT maxIndices = 20; // インデックスバッファは使わない場合

    // ライン描画用のメッシュを作成
    line_ = CreateMesh(maxVertices, maxIndices);
}

std::unique_ptr<PrimitiveDrawer::Mesh> PrimitiveDrawer::CreateMesh(UINT vertexCount, UINT indexCount){
    auto mesh = std::make_unique<Mesh>();

    // 頂点バッファのサイズを設定
    UINT vertexBufferSize = sizeof(VertexPosColor) * vertexCount;

    // 頂点バッファのリソースを作成
    mesh->vertBuff = CreateBufferResource(device_.Get(), vertexBufferSize);

    // 頂点バッファのビューを設定
    mesh->vbView.BufferLocation = mesh->vertBuff->GetGPUVirtualAddress();
    mesh->vbView.StrideInBytes = sizeof(VertexPosColor);
    mesh->vbView.SizeInBytes = vertexBufferSize;

    // 頂点バッファをマップ
    D3D12_RANGE readRange = {0, 0}; // CPUによる読み取りは行わない
    mesh->vertBuff->Map(0, &readRange, reinterpret_cast< void** >(&mesh->vertMap));

    // インデックスバッファのサイズを設定
    UINT indexBufferSize = sizeof(uint16_t) * indexCount;

    // インデックスバッファのリソースを作成
    if (indexCount > 0){
        mesh->indexBuff = CreateBufferResource(device_.Get(), indexBufferSize);

        // インデックスバッファのビューを設定
        mesh->ibView.BufferLocation = mesh->indexBuff->GetGPUVirtualAddress();
        mesh->ibView.Format = DXGI_FORMAT_R16_UINT;
        mesh->ibView.SizeInBytes = indexBufferSize;

        // インデックスバッファをマップ
        mesh->indexBuff->Map(0, &readRange, reinterpret_cast< void** >(&mesh->indexMap));
    }

    return mesh;
}

void PrimitiveDrawer::DrawLine3d(const Vector3& p1, const Vector3& p2, const Vector4& color){
    // 使用可能な最大線分数を超えていないか確認
    if (indexLine_ < kMaxLineCount){
        // ラインの頂点データをバッファに追加
        line_->vertMap[indexLine_ * 2] = {p1, color};
        line_->vertMap[indexLine_ * 2 + 1] = {p2, color};

        // 次のインデックスに進む
        indexLine_++;
    }
}

void PrimitiveDrawer::Render(){
    if (indexLine_ == 0) return; // 描画する線がない場合は終了

    UpdateMatrixBuffer();

    // パイプラインステートとルートシグネチャを設定
    commandList_->SetPipelineState(pipelineState_.Get());
    commandList_->SetGraphicsRootSignature(rootSignature_.Get());

    // プリミティブトポロジをラインリストに設定
    commandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);

    // 頂点バッファを設定
    D3D12_VERTEX_BUFFER_VIEW vbView = line_->vbView;
    commandList_->IASetVertexBuffers(0, 1, &vbView);
    // wvp用のCBufferの場所を設定
    commandList_->SetGraphicsRootConstantBufferView(0, wvpResource_->GetGPUVirtualAddress());

    // ラインを描画
    commandList_->DrawInstanced(indexLine_ * 2, 1, 0, 0);

    // `indexLine_`をリセット
    indexLine_ = 0;
}

void PrimitiveDrawer::CreateMatrixBuffer(){
    wvpResource_ = CreateBufferResource(device_.Get(), sizeof(TransformationMatrix));
    wvpResource_->Map(0, nullptr, reinterpret_cast< void** >(&matrixData_));
}

void PrimitiveDrawer::UpdateMatrixBuffer(){
    if (!viewProjection_) return;

    // ワールド行列（必要に応じて設定）
    Matrix4x4 worldMatrix = Matrix4x4::MakeIdentity();
    Matrix4x4 worldViewProjectionMatrix = Matrix4x4::Multiply(worldMatrix, viewProjection_->GetViewProjection());

    // 定数バッファの更新
    matrixData_->world = worldMatrix;
    matrixData_->WVP = worldViewProjectionMatrix;
}