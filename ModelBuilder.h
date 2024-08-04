#pragma once
#include"Model.h"
#include"ViewProjection.h"

#include<unordered_map>
#include<memory>

#include<string>

/// <summary>
/// モデル作成クラス
/// </summary>
class ModelBuilder{
public:
	ModelBuilder() = default;
	~ModelBuilder() = default;

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();
	
	/// <summary>
	/// 更新
	/// </summary>
	void Update();
	
	/// <summary>
	/// 描画
	/// </summary>
	void Draw();
	
	/// <summary>
	/// モデル追加
	/// </summary>
	/// <param name="modelPath"></param>
	void AddModel(const std::string& modelPath);

	/// <summary>
	/// モデル削除
	/// </summary>
	/// <param name="modelPath"></param>
	/// <param name="index"></param>
	void RemoveModel(const std::string& modelPath, size_t index);
	
	/// <summary>
	/// imguiの描画
	/// </summary>
	void ShowImGuiInterface();
	
	/// <summary>
	/// viewProjectionのセット
	/// </summary>
	/// <param name="viewProjection"></param>
	void SetViewProjection(ViewProjection* viewProjection);

private:
	std::unique_ptr<Model>CreateModel(const std::string& modelPath);

private:
	//作成したモデル
	std::vector<std::pair<std::string, std::unique_ptr<Model>>> models_;
	ViewProjection* viewProjection_;
};

