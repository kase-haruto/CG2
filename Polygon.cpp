#include "Polygon.h"
#include"DirectXCommon.h"
#include"MyFunc.h"

Polygon::Polygon(){}

Polygon::~Polygon(){}

void Polygon::Initialize(){
	dxCommon_ = DirectXCommon::GetInstance();
	device_ = dxCommon_->GetDevice();
	commandList_ = dxCommon_->GetCommandList();
	//パイプライン
	pipeline_ = std::make_unique<Pipeline>();
	pipeline_->Initialize(device_.Get());
	vertexBufferResource_ = CreateBufferResource(device_.Get(), sizeof(VertexData) * 6);
	wTransform_ = {{1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{1.0f,1.0f,1.0f}};
}

void Polygon::CreateVertexBufferView(){
	//リソースの先頭のアドレスから使う
	vertexBufferView.BufferLocation = vertexBufferResource_->GetGPUVirtualAddress();
	//使用するリソースのサイズは頂点3つ分のサイズ
	vertexBufferView.SizeInBytes = sizeof(VertexData) * 6;
	//1頂点当たりのサイズ
	vertexBufferView.StrideInBytes = sizeof(VertexData);
}

void Polygon::Update(){

}

void Polygon::Draw(){
	//ビューポート
	D3D12_VIEWPORT viewport = dxCommon_->GetViewport();
	//シザー矩形
	D3D12_RECT scissorRect = dxCommon_->GetScissorRect();
	
	commandList_->RSSetViewports(1, &viewport);
	commandList_->RSSetScissorRects(1, &scissorRect);
	commandList_->SetGraphicsRootSignature(pipeline_->GetRootSignature());
	commandList_->SetPipelineState(pipeline_->GetPipelineState());
	commandList_->IASetVertexBuffers(0, 1, &vertexBufferView);
	//形状を設定。psoに設定しているものとはまた別。同じものを設定すると考える
	commandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}
