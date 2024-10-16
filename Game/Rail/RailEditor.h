#pragma once

#include"Model.h"

#include<memory>
#include<vector>

class RailEditor{
public:
	RailEditor();
	~RailEditor();

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
	/// Railの線を描画
	/// </summary>
	void DrawLine();

	/// <summary>
	/// ポインタを借りてくる
	/// </summary>
	void SetViewProjection(const ViewProjection* viewProjection);

	/// <summary>
	/// レールの取得
	/// </summary>
	/// <returns></returns>
	const std::vector<Vector3>& GetControlPoint()const{ return ctrlPoints_; }

private:
	/// <summary>
	/// 制御点の追加
	/// </summary>
	void AddCtrlPoint(const Vector3& preCtrlPoint, size_t insertIndex);

	/// <summary>
	/// 制御点をjsonに保存
	/// </summary>
	void SaveControlPointToJson();

	/// <summary>
	/// 制御点をjsonから読み込む
	/// </summary>
	void LoadControlPointFromJson();

	/// <summary>
	/// Railのモデルを追加
	/// </summary>
	void AddNewRailModel(const Vector3& spawnPos);


private:

	//////////////////////////////////////////////////////////
	//		メンバ変数
	/////////////////////////////////////////////////////////
	std::vector<Vector3>ctrlPoints_;
	std::vector<std::unique_ptr<Model>> railModels_;
	// 選択された制御点のインデックス
	int selectedCtrlPoint_ = -1;

	//////////////////////////////////////////////////////////
	//		debug用
	/////////////////////////////////////////////////////////
	//デバッグ表示用モデル
	std::vector < std::unique_ptr<Model>>debugModels_;
	//モデル表示用カメラ
	const ViewProjection* pViewProjection_ = nullptr;
};