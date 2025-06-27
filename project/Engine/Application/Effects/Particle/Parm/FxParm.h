#pragma once
/* ========================================================================
/*	include space
/* ===================================================================== */

// engine
#include <Engine/Foundation/Utility/Random/Random.h>
#include <Data/Engine/Configs/Scene/Objects/Particle/FxParmConfig.h>

template<typename T>
class FxParam{
public:
	FxParam() = default;
	//===================================================================*/
	//					public methods
	//===================================================================*/
	void SetConstant(const T& value);
	void SetRandom(const T& min, const T& max);
	T Get() const;

	FxValueMode GetMode() const{ return mode_; }
	const T& GetMin() const{ return min_; }
	const T& GetMax() const{ return max_; }
	const T& GetConstant() const{ return constant_; }

	//config
	void FromConfig(const FxParamConfig<T>& cfg);
	FxParamConfig<T> ToConfig() const;
private:
	FxValueMode mode_ = FxValueMode::Constant;
	T constant_ {};	//< 定数値
	T min_ {};		//< 最小値（ランダムモード用）
	T max_ {};		//< 最大値（ランダムモード用）
};

/////////////////////////////////////////////////////////////////////////////////////////
//		定数の設定
/////////////////////////////////////////////////////////////////////////////////////////
template<typename T>
inline void FxParam<T>::SetConstant(const T& value){
	mode_ = FxValueMode::Constant;
	constant_ = value;
}

/////////////////////////////////////////////////////////////////////////////////////////
//		ランダム数の設定
/////////////////////////////////////////////////////////////////////////////////////////
template<typename T>
inline void FxParam<T>::SetRandom(const T& min, const T& max){
	mode_ = FxValueMode::Random;
	min_ = min;
	max_ = max;
}

/////////////////////////////////////////////////////////////////////////////////////////
//		値の取得（スカラー型用）
/////////////////////////////////////////////////////////////////////////////////////////
template<typename T>
inline T FxParam<T>::Get() const{
	if (mode_ == FxValueMode::Constant){
		return constant_;
	} else{
		return Random::Generate<T>(min_, max_);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
//		値の取得（Vector3 専用特殊化）
/////////////////////////////////////////////////////////////////////////////////////////
template<>
inline Vector3 FxParam<Vector3>::Get() const{
	if (mode_ == FxValueMode::Constant){
		return constant_;
	} else{
		return Random::GenerateVector3(min_, max_);
	}
}

//===================================================================*/
//		Config からの設定
//===================================================================*/
template<typename T>
inline void FxParam<T>::FromConfig(const FxParamConfig<T>& cfg){
	mode_ = cfg.mode;
	constant_ = cfg.constant;
	min_ = cfg.min;
	max_ = cfg.max;
}

//===================================================================*/
//		Config への変換
//===================================================================*/
template<typename T>
inline FxParamConfig<T> FxParam<T>::ToConfig() const{
	return FxParamConfig<T>{ mode_, constant_, min_, max_ };
}