#pragma once

#include <random>
#include <type_traits>

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
};
