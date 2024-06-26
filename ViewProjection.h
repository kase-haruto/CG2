﻿#pragma once
#include"Transform.h"
#include"Matrix4x4.h"
#include<stdint.h>
#include<d3d12.h>
#include<DirectXMath.h>
#include<wrl.h>

using namespace Microsoft::WRL;

//定数バッファ用data構造体
struct ConstBufferDataViewProjection{
	Matrix4x4 view;			//ワールド→ビュー変換行列
	Matrix4x4 projection;	//ビュー→プロジェクション変換行列
	Vector3 cameraPos;		//カメラ座標(ワールド座標)
};

class DirectXCommon;

class ViewProjection{
public:

#pragma region ビュー行列の設定
	Transform transform = {{1.0f,1.0f,1.0f},
						   {0.0f,0.0f,0.0f},
						   {0.0f,0.0f,-5.0f}
	};
#pragma endregion

#pragma region 射影行列の設定
	// 垂直方向視野角
	float fovAngleY = 45.0f * 3.141592654f / 180.0f;
	// ビューポートのアスペクト比
	float aspectRatio = ( float ) 16 / 9;
	// 深度限界（手前側）
	float nearZ = 0.1f;
	// 深度限界（奥側）
	float farZ = 1000.0f;
#pragma endregion

	Matrix4x4 viewProjection_;
	Matrix4x4 matView;
	Matrix4x4 matProjection;

private:
	//定数バッファ
	ComPtr<ID3D12Resource>constBuffer_;

	//DirectX関連
	DirectXCommon* dxCommon_;
	ID3D12Device* device_;
	ID3D12GraphicsCommandList* commandList_;

	ConstBufferDataViewProjection* camearData_;

private:
	Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip);

public:


	ViewProjection(DirectXCommon* directX);
	~ViewProjection();

	void ImGui();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="width"></param>
	/// <param name="height"></param>
	void Initialize();
	/// <summary>
	/// 定数バッファ生成
	/// </summary>
	void CreateConstBuffer();
	/// <summary>
	/// マッピングする
	/// </summary>
	void Map();
	/// <summary>
	/// 行列を更新
	/// </summary>
	void UpdateMatrix();
	/// <summary>
	/// ビュー行列を更新
	/// </summary>
	void UpdateViewMatrix();
	/// <summary>
	/// 射影行列を更新
	/// </summary>
	void UpdateProjectionMatrix();
	/// <summary>
	/// 定数バッファの取得
	/// </summary>
	/// <returns></returns>
	ComPtr<ID3D12Resource>GetConstBuffer(){ return constBuffer_; }

	Vector3 GetPos()const{
		Vector3 pos;
		pos = transform.translate;
		return pos;
	}


	Matrix4x4 GetViewProjection()const{ return viewProjection_; }
};