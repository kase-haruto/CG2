#include "Ease.h"

// Linear
float EvoEase::Linear(float t){
	return t;
}

// Quad
float EvoEase::EaseInQuad(float t){
	return t * t;
}

float EvoEase::EaseOutQuad(float t){
	return t * (2 - t);
}

float EvoEase::EaseInOutQuad(float t){
	return t < 0.5f ? 2 * t * t : -1 + (4 - 2 * t) * t;
}

// Cubic
float EvoEase::EaseInCubic(float t){
	return t * t * t;
}

float EvoEase::EaseOutCubic(float t){
	return (--t) * t * t + 1;
}

float EvoEase::EaseInOutCubic(float t){
	return t < 0.5f ? 4 * t * t * t : (t - 1) * (2 * t - 2) * (2 * t - 2) + 1;
}

// Sine
float EvoEase::EaseInSine(float t){
	return 1 - std::cos((t * std::numbers::pi_v<float>) / 2);
}

float EvoEase::EaseOutSine(float t){
	return std::sin((t * std::numbers::pi_v<float>) / 2);
}

float EvoEase::EaseInOutSine(float t){
	return -(std::cos(std::numbers::pi_v<float> *t) - 1) / 2;
}

// Exponential
float EvoEase::EaseInExpo(float t){
	return t == 0 ? 0 : std::powf(2, 10 * (t - 1));
}

float EvoEase::EaseOutExpo(float t){
	return t == 1 ? 1 : 1 - std::powf(2, -10 * t);
}

float EvoEase::EaseInOutExpo(float t){
	if (t == 0) return 0;
	if (t == 1) return 1;
	return t < 0.5f
		? std::powf(2, 10 * (2 * t - 1)) / 2
		: (2 - std::powf(2, -10 * (2 * t - 1))) / 2;
}

// Back
float EvoEase::EaseInBack(float t, float s){
	return t * t * ((s + 1) * t - s);
}

float EvoEase::EaseOutBack(float t, float s){
	return (--t) * t * ((s + 1) * t + s) + 1;
}

float EvoEase::EaseInOutBack(float t, float s){
	t *= 2;
	s *= 1.525f;
	if (t < 1){
		return 0.5f * (t * t * ((s + 1) * t - s));
	} else{
		t -= 2;
		return 0.5f * (t * t * ((s + 1) * t + s) + 2);
	}
}
