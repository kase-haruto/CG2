#pragma once

#include "BaseModel.h"

class Model
    : public BaseModel{
public:
    Model() = default;
    Model(const std::string& fileName);
    ~Model() override;

    //================================
    // BaseModel の純粋仮想関数の実装
    //================================
    void Initialize() override;
    void Create(const std::string& filename) override;
    void Update() override;
    void UpdateMatrix() override;
    void Map() override;
    void ShowImGuiInterface() override;
    void Draw() override;

    //=============
    // Transform関連
    //=============
    void SetPos(const Vector3& pos) override{ transform.translate = pos; }
    void SetSize(const Vector3& size) override{ transform.scale = size; }
    void SetUvScale(const Vector3& uvScale) override{ uvTransform.scale = uvScale; }
    void SetColor(const Vector4& color) override{ RGBa = color; }
    const Vector4& GetColor() const override{ return RGBa; }

private:
    //============
    // バッファ生成/マップの実装
    //============
    void CreateMaterialBuffer() override;
    void CreateMatrixBuffer() override;
    void MaterialBufferMap() override;
    void MatrixBufferMap() override;
};
