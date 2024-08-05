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
	~ModelBuilder();

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
	void RemoveModel(size_t index);
	
	/// <summary>
	/// imguiの描画
	/// </summary>
	void ShowImGuiInterface();
	
	/// <summary>
	/// viewProjectionのセット
	/// </summary>
	/// <param name="viewProjection"></param>
	void SetViewProjection(ViewProjection* viewProjection);

	/// <summary>
	/// modelファイルを読み込む
	/// </summary>
	/// <param name="filePath"></param>
	void LoadModel(const std::string& filePath);
	
	/// <summary>
	/// 
	/// </summary>
	/// <param name="directoryPath"></param>
	void ListAndLoadModelsFromDirectory(const std::string& directoryPath);

private:
	std::unique_ptr<Model>CreateModel(const std::string& modelPath);


private:
	//作成したモデル
	std::vector<std::pair<std::string, std::unique_ptr<Model>>> models_;
	std::vector<std::string> availableModels_;
	ViewProjection* viewProjection_;
};

