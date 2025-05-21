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
	void Draw(const WorldTransform& transform)override;
	void ShowImGuiInterface() override;

	//=============
	// Transform関連
	//=============

private:
	//============
	// バッファ生成/マップの実装
	//============
	void CreateMaterialBuffer() override;

	void MaterialBufferMap()override;

};
