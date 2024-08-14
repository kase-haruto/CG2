#pragma once 
#include"Vector3.h"
#include"ViewProjection.h"

#include<stdint.h>
#include<string>


class AABB{

public:
	AABB() = default;
	~AABB() = default;

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="min"></param>
	/// <param name="max"></param>
	void Initialize(const Vector3& min, const Vector3& max);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// uiの更新
	/// </summary>
	void UpdateUI(std::string lavel);

	Vector3 GetMin()const;
	Vector3 GetMax()const;

	void SetMin(const Vector3 min);
	void SetMax(const Vector3 max);

	uint32_t GetColor()const;
	void SetColor(const uint32_t color);

public:
	Vector3 min;
	Vector3 max;
	uint32_t color;

};

