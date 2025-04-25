#pragma once
#include <random>
#include <type_traits>
#include "lib/myMath/Vector3.h" // Vector3 クラスが定義されている前提

class Random{
public:
	Random(){}
	~Random(){}

	template <typename T>
	static T Generate(T min, T max){
		static_assert(
			std::is_integral<T>::value || std::is_floating_point<T>::value,
			"Only integral or floating-point types are supported."
			);

		static thread_local std::random_device randomDevice;
		static thread_local std::mt19937 randomEngine {randomDevice()};

		if constexpr (std::is_integral<T>::value){
			std::uniform_int_distribution<T> dist(min, max);
			return dist(randomEngine);
		} else{
			std::uniform_real_distribution<T> dist(min, max);
			return dist(randomEngine);
		}
	}

	static Vector3 GenerateVector3(float min, float max){
		return Vector3(
			Generate<float>(min, max),
			Generate<float>(min, max),
			Generate<float>(min, max)
		);
	}

	static Vector3 GenerateUnitVector3(){
		float x = Generate<float>(-1.0f, 1.0f);
		float y = Generate<float>(-1.0f, 1.0f);
		float z = Generate<float>(-1.0f, 1.0f);
		Vector3 v(x, y, z);

		// 正規化して単位ベクトルを返す
		return v.Normalize();
	}
};
