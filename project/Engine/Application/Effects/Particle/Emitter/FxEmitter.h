#pragma once
/* ========================================================================
/*	include space
/* ===================================================================== */

// engine
#include <Engine/Application/Effects/Particle/Parm/FxParm.h>

// c++
#include <vector>

// forward declaration
struct Vector3;
struct FxUnit;

/* ========================================================================
/*	particle emitter
/* ===================================================================== */
class FxEmitter{
public:
	//===================================================================*/
	//					public func
	//===================================================================*/
	FxEmitter() = default;
	~FxEmitter() = default;

	void Update(float deltaTime);
	void ResetFxUnit(FxUnit& fxUnit);

	//--------- accessor -------------------------------------------------//
	const std::vector<FxUnit>& GetUnits()const { return units_; }
	const std::string& GetModelPath() const{ return modelPath; }

private:
	//===================================================================*/
	//					private func
	//===================================================================*/
	void Emit();

private:
	//===================================================================*/
	//					private variable
	//===================================================================*/
	std::string modelPath = "plane.obj";	//< モデルパス（デフォルトは平面）
	const int kMaxUnits_ = 1024;			//< 最大パーティクル数
	int unitCount_ = 0;						//< 現在のパーティクル数
	std::vector<FxUnit> units_;				//< パーティクルユニットの配列
	Vector3 position_;						//< エミッターの位置

	float emitRate_ = 0.1f;					//< パーティクル生成レート
	float emitTimer_ = 0.0f;				//< パーティクル生成タイマー

	FxParam<Vector3> velocity_;
	FxParam<float> lifetime_;

};