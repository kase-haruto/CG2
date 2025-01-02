#pragma once
#include "IPlayerAttack.h"

class WeakDiagonalSlash :
    public IPlayerAttack{
public:
	//===================================================================*/
	//                   public member functions
	//===================================================================*/
	WeakDiagonalSlash(const std::string& attackName);
	~WeakDiagonalSlash() override = default;

	/* main ===================================*/
	void Initialize()override;	//< 初期化
	void Execution()override;	//< 実行
	void Update()override;		//< 更新
	void Draw()override;		//< 描画

	/* helper ==================================*/
	std::unique_ptr<IPlayerAttack> Clone() const override;

	// 制御点のゲッター
	std::vector<Vector3>& GetControlPoints(){ return controlPoints_; }
	const std::vector<Vector3>& GetControlPoints() const{ return controlPoints_; }

	// 軌道の保存・読み込み
	void SaveControlPoints(const std::string& filepath) const;
	void LoadControlPoints(const std::string& filepath);


	/* gui ui ===================================*/
	void ShowGui()override;		//< gui表示

	/* collision ===================================*/
	void OnCollisionEnter(Collider* other)override;
	void OnCollisionStay(Collider* other)override;
	void OnCollisionExit(Collider* other)override;

private:
	//===================================================================*/
	//                   private member functions
	//===================================================================*/
	void SetupDefaultControlPoints();

private:
	//===================================================================*/
	//                   private member variables
	//===================================================================*/
	std::vector<Vector3> controlPoints_;	//< Catmull-Rom用の制御点
	float animationTime_;					//< 現在の時間パラメータ (0.0 〜 1.0)
	float animationSpeed_;					//< アニメーションの速度
	Vector3 currentPosition_;				//< 現在の位置
};

