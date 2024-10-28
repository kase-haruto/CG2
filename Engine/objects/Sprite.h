#pragma once
#include<d3d12.h>
#include<wrl.h>
#include"Transform.h"
#include"Matrix4x4.h"
#include"Material.h"
#include "Vector2.h"
#include "Vector4.h"
#include "VertexData.h"

using namespace Microsoft::WRL;

class DirectXCommon;

/// <summary>
/// スプライト
/// </summary>
class Sprite{
public:
	Sprite();
	~Sprite();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(const std::string& filePath);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	void UpdateMatrix();

	/// <summary>
	/// 行列の更新
	/// </summary>
	void UpdateMaterix();

	/// <summary>
	/// トランスフォームの更新
	/// </summary>
	void UpdateTransform();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	/// <summary>
	/// 定数バッファの作成
	/// </summary>
	void CreateBuffer();

	/// <summary>
	/// マップ
	/// </summary>
	void Map();
	void IndexResourceMap();
	void VertexResourceMap();
	void TransformResourceMap();
	void MaterialResourceMap();

	/// <summary>
	/// 定数バッファの取得
	/// </summary>
	/// <returns></returns>
	ComPtr<ID3D12Resource>GetConstBuffer(){ return vertexResource_; }

	/// <summary>
	/// 回転
	/// </summary>
	/// <param name="rotation"></param>
	void SetRotation(float rotation){ this->rotate = rotation; }
	float GetRotation()const{ return rotate; }

	/// <summary>
	/// 座標
	/// </summary>
	/// <param name="position"></param>
	void SetPosition(const Vector2& position){ this->position = position; }
	const Vector2& GetPosition()const{ return position; }

	/// <summary>
	/// 色
	/// </summary>
	/// <returns></returns>
	const Vector4& GetColor()const{ return materialData_->color; }
	void SetColor(const Vector4& color){ materialData_->color = color; }

	/// <summary>
	/// サイズ
	/// </summary>
	/// <returns></returns>
	const Vector2& GetSize()const{ return size; }
	void SetSize(const Vector2& size){ this->size = size; }

	/// <summary>
	/// アンカーポイント
	/// </summary>
	/// <returns></returns>
	const Vector2& GetAnchorPoint()const{ return anchorPoint; }
	void SetAnchorPoint(const Vector2& anchorPoint){ this->anchorPoint = anchorPoint; }

	/// <summary>
	/// 左上座標
	/// </summary>
	/// <returns></returns>
	const Vector2& GetLeftTop()const{ return textureLeftTop; }
	void SetLeftTop(const Vector2& leftTop){ this->textureLeftTop = leftTop; }

private:
	//座標
	Vector2 position {0.0f,0.0f};
	//回転
	float rotate = 0.0f;
	//色
	Vector4 color ={1.0f,1.0f,1.0f,1.0f};
	//size
	Vector2 size = {640.0f,360.0f};
	//アンカーポイント
	Vector2 anchorPoint = {0.0f,0.0f};
	//テクスチャ左上座標
	Vector2 textureLeftTop = {0.0f,0.0f};
	//テクスチャ切り出しサイズ
	Vector2 textureSize = {100.0f,100.0f};

	std::string path;

#pragma region

private:
	///=============================================================
	///	リソース
	///=============================================================
	//viewの生成
	D3D12_INDEX_BUFFER_VIEW indexBufferView {};
	ComPtr<ID3D12Resource>vertexResource_;
	ComPtr<ID3D12Resource>indexResource_;
	ComPtr<ID3D12Resource>transformResource_;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferViewSprite {};

	//directX関連
	ComPtr<ID3D12Device> device_;
	ComPtr<ID3D12GraphicsCommandList>commandList_;
	Microsoft::WRL::ComPtr<ID3D12RootSignature>rootSignature_;
	Microsoft::WRL::ComPtr<ID3D12PipelineState>pipelineState_;

	Transform transform_ {{1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f}};
	Transform uvTransform {{1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f}};
	Matrix4x4* transformData = nullptr;

	//マテリアル用のリソース
	ComPtr<ID3D12Resource> materialResource_;
	VertexData* vertexData = nullptr;
	Material2D* materialData_;

#pragma endregion

private:
	///=============================================================
	///	texture
	///=============================================================
	D3D12_GPU_DESCRIPTOR_HANDLE handle;
};
