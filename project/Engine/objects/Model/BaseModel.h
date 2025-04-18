#pragma once

#include "engine/objects/ModelData.h"
#include "engine/graphics/Material.h"
#include "engine/objects/TransformationMatrix.h"
#include "engine/objects/Transform.h"
#include "lib/myMath/Vector4.h"
#include "Engine/graphics/blendMode/BlendMode.h"
#include "Engine/core/DirectX/Buffer/DxConstantBuffer.h"

#include <d3d12.h>
#include <wrl.h>
#include <string>
#include <memory>

class BaseModel{
public:
    virtual ~BaseModel() = default;

    //=============
    // 仮想関数一覧
    //=============
    /// 初期化
    virtual void Initialize() = 0;
    /// 毎フレームの更新
    virtual void Update();
    virtual void OnModelLoaded();
    virtual void AnimationUpdate(){}
    /// 行列更新
    virtual void UpdateMatrix();
	void UpdateTexture();
    /// バッファへのMap操作(リソースへのCPU書き込みなど)
    virtual void Map() = 0;
    /// ImGuiの更新処理
    virtual void ShowImGuiInterface();
    /// 描画
    virtual void Draw();

    //=============
    // Transform関連
    //=============
    virtual void SetUvScale(const Vector3& uvScale) = 0;
    virtual void SetColor(const Vector4& color) = 0;
    virtual const Vector4& GetColor() const = 0;
	const WorldTransform& GetWorldTransform(){ return worldTransform_; }

protected:
    //=============
    // DirectX関連
    //=============
    Microsoft::WRL::ComPtr<ID3D12Device>              device_;
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList_;

    //=============
    // リソース・バッファビュー
    //=============

	DxConstantBuffer<Material> materialBuffer_;

    //=============
    // テクスチャ
    //=============
    std::optional<D3D12_GPU_DESCRIPTOR_HANDLE> handle_ {};

    //=============
    // モデルデータ・マテリアル
    //=============
	std::string fileName_;
    std::optional<ModelData> modelData_;
    Material materialData_;
    Material materialParameter_;
    TransformationMatrix matrixData_;

public:
    //=============
    // 各種パラメータ
    //=============
	BlendMode blendMode_ = BlendMode::NORMAL;
    Vector4    RGBa = {1.0f, 1.0f, 1.0f, 1.0f};
    EulerTransform  uvTransform {{1.0f, 1.0f, 1.0f},
                             {0.0f, 0.0f, 0.0f},
                             {0.0f, 0.0f, 0.0f}};

    WorldTransform worldTransform_;

protected:
    // 既存のメンバ変数やメソッドの他に以下を追加
    std::vector<D3D12_GPU_DESCRIPTOR_HANDLE> textureHandles_; // テクスチャハンドルリスト
    float animationSpeed_ = 0.1f; // アニメーションの速度 (秒/フレーム)
    float elapsedTime_ = 0.0f; // 経過時間
    size_t currentFrameIndex_ = 0; // 現在のフレームインデックス

public:
	Matrix4x4 GetWorldRotationMatrix();

protected:
    /// リソースを生成する際のディレクトリパス(例)
    static const std::string directoryPath_;

    //============
    // バッファ作成/マッピング(派生先で使う)
    //============
    virtual void CreateMaterialBuffer() = 0;
    virtual void MaterialBufferMap() = 0;
};
