#pragma once


/// <summary>
/// シーンの基底クラス
/// </summary>
class IScene{
public:
	IScene();
	virtual ~IScene();

	/// <summary>
	/// 初期化
	/// </summary>
	virtual void Initialize() = 0;
	/// <summary>
	/// 更新
	/// </summary>
	virtual void Update() = 0;
	/// <summary>
	/// 描画
	/// </summary>
	virtual void Draw() = 0;
	/// <summary>
	/// 解放
	/// </summary>
	virtual void Finalize() = 0;

};

