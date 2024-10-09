#pragma once

#include"Model.h"

#include<memory>
#include<vector>

class RailEditor{
public:
	RailEditor();
	~RailEditor() = default;

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


private:

	//////////////////////////////////////////////////////////
	//		メンバ変数
	/////////////////////////////////////////////////////////
	std::vector<Vector3>ctrlPoints_;
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
