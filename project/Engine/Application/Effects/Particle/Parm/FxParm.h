#pragma once
/* ========================================================================
/*	include space
/* ===================================================================== */

// engine
#include <Engine/Foundation/Utility/Random/Random.h>

enum class FxValueMode{
	Constant,
	Random
};

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
//		値の取得
/////////////////////////////////////////////////////////////////////////////////////////
template<typename T>
inline T FxParam<T>::Get() const{
	if (mode_ == FxValueMode::Constant){
		return constant_;
	} else{
		return Random::Generate(min_, max_);
	}
}