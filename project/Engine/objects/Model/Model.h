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
	void InitializeTextures(const std::vector<std::string>& textureFilePaths);
	void Map() override;
	void Draw()override;
	void ShowImGuiInterface() override;

	//=============
	// Transform関連
	//=============
	void SetSize(const Vector3& size){ worldTransform_.scale = size; }
	void SetUvScale(const Vector3& uvScale) override{ uvTransform.scale = uvScale; }
	void SetColor(const Vector4& color) override{ RGBa = color; }
	const Vector4& GetColor() const override{ return RGBa; }

private:
	//============
	// バッファ生成/マップの実装
	//============
	void CreateMaterialBuffer() override;

	void MaterialBufferMap()override;

private:


public:
};
