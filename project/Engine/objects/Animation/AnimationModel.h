#pragma once
#include "../Model/BaseModel.h"

#include "AnimationStruct.h"

class AnimationModel
    : public BaseModel{
public:
    //===================================================================*/
	//                   public func
    //===================================================================*/
    AnimationModel() = default;
    AnimationModel(const std::string& fileName);
    ~AnimationModel() override = default;

    //================================
    // BaseModel の純粋仮想関数の実装
    //================================
    void Initialize() override;
    void Create(const std::string& filename) override;
    void Update() override;
    void Draw() override;


    void UpdateMatrix() override;
    void Map() override;
    void ShowImGuiInterface() override;

    //=============
    // Transform関連
    //=============
    void SetPos(const Vector3& pos) override{ transform.translate = pos; }
    void SetSize(const Vector3& size) override{ transform.scale = size; }
    void SetUvScale(const Vector3& uvScale) override{ uvTransform.scale = uvScale; }
    void SetColor(const Vector4& color) override{ RGBa = color; }
    const Vector4& GetColor() const override{ return RGBa; }

private:
    //===================================================================*/
	//                   private func
    //===================================================================*/
    //============
    // バッファ生成/マップの実装
    //============
    void CreateMaterialBuffer() override;
    void CreateMatrixBuffer() override;
    void MaterialBufferMap() override;
    void MatrixBufferMap() override;

	//* animation *//
	void PlayAnimation();

private:
    //===================================================================*/
	//                    private variables
    //===================================================================*/
	float animationTime_ = 0.0f;    //<アニメーションの経過時間
	Matrix4x4 animationMatrix_;	    //<アニメーション行列
	Animation animation_;           //<アニメーションデータ
};
