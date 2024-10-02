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
	/// ポインタを借りてくる
	/// </summary>
	void SetViewProjection(const ViewProjection* viewProjection);

private:
	/// <summary>
	/// 制御点の追加
	/// </summary>
	void AddCtrlPoint(const Vector3& preCtrlPoint);

private:
	//デバッグ表示用モデル
	std::vector < std::unique_ptr<Model>>debugModels_;
	//モデル表示用カメラ
	const ViewProjection* pViewProjection_ = nullptr;
};
