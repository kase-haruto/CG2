#pragma once 

/* engine */
#include <Engine/Foundation/Math/Vector3.h>

/* c++ */
#include<stdint.h>
#include<string>

struct Vector3;

class AABB {

public:
	AABB(const Vector3& min, const Vector3& max, uint32_t color = 0xFFFFFFFF)
		: min_(min), max_(max), color(color) {}
	AABB() = default;
	~AABB() = default;

	void Initialize(const Vector3& min, const Vector3& max);
	void Update();
	void UpdateUI(std::string lavel);

	Vector3 GetMin()const;
	Vector3 GetMax()const;

	void SetMin(const Vector3 min);
	void SetMax(const Vector3 max);

	uint32_t GetColor()const;
	void SetColor(const uint32_t color);

public:
	Vector3 min_;
	Vector3 max_;
	uint32_t color;

};

