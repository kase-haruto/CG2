#pragma once
#include<d3d12.h>
#include"DirectXCommon.h"
#include<wrl.h>
#include"Transform.h"
#include"Pipeline.h"
#include<memory>
class DirectXCommon;

class Polygon{
private:
	DirectXCommon* dxCommon_;
	Microsoft::WRL::ComPtr<ID3D12Device>device_;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>commandList_;
	
	Transform wTransform_;
	Microsoft::WRL::ComPtr <ID3D12Resource>vertexBufferResource_;
	//頂点バッファビューを作成する
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView {};
	std::unique_ptr<Pipeline> pipeline_;
public:
	Polygon();
	~Polygon();
	void Initialize();
	void Update();
	void Draw();

	void CreateVertexBufferView();

};

