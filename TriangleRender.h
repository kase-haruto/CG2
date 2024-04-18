#pragma once
#include <d3dcompiler.h>
#include<d3d12.h>

class TriangleRender final{
public://メンバ関数
	
	TriangleRender();
	~TriangleRender();

	/// <summary>
	/// vertexResourceの作成
	/// </summary>
	/// <param name="device"></param>
	void CreateVertexResource(ID3D12Device* device);

	/// <summary>
	/// vertexBufferViewの作成
	/// </summary>
	void CreateVertexBufferView();
	
	/// <summary>
	/// Resourceにデータを書き込む
	/// </summary>
	void UploadVertexData();

private://メンバ変数
	ID3D12Resource* vertexResource = nullptr;
};

