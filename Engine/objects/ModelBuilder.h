#pragma once
#include"Model.h"
#include"ViewProjection.h"
#include "Editor/BaseEditor.h"

#include<unordered_map>
#include<memory>

#include<string>
#include<map>

/// <summary>
/// モデル作成クラス
/// </summary>
class ModelBuilder:
public BaseEditor{
public:
	ModelBuilder();
	~ModelBuilder()override = default;

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
	void ShowImGuiInterface()override;
	
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


	const char* GetEditorName() const override{
		return "ModelBuilder";
	}

private:
	// モデルの保存と読み込み
	void SaveModels(const std::string& filePath);
	void LoadModels(const std::string& filePath);

private:
	std::unique_ptr<Model>CreateModel(const std::string& modelPath);


private:
	// モデル名（ユニーク）とモデルオブジェクトのペア
	std::map<std::string, std::pair<std::string, std::unique_ptr<Model>>> models_;
	// キー: ユニーク名 (unique_key)
	// 値: <オリジナル名, モデルオブジェクト>

	std::vector<std::string> availableModels_;
	ViewProjection* viewProjection_;

	// モデル選択用
	std::string selectedModelName_;
};

