#include"myFunc/MathFunc.h"

#include<cassert>
#include<cmath>
#include<algorithm>

Vector3 CatmullRomInterpolation(
	const Vector3& p0, const Vector3& p1, const Vector3& p2, const Vector3& p3, float t){

	float t2 = t * t;
	float t3 = t2 * t;

	Vector3 e3 = (-1 * p0) + 3 * p1 - 3 * p2 + p3;
	Vector3 e2 = 2 * p0 - 5 * p1 + 4 * p2 - p3;
	Vector3 e1 = (-1 * p0) + p2;
	Vector3 e0 = 2 * p1;

	return (e3 * t3 + e2 * t2 + e1 * t + e0) * 0.5f;
}

Vector3 CatmullRomPosition(const std::vector<Vector3>& points, float t){
	assert(points.size() >= 4 && "制御点は4以上が必要です");

	size_t division = points.size() - 1; // 区間数は制御点の数 - 3
	float areaWidth = 1.0f / division; // 各区間の長さ

	// 区間番号
	float t_2 = std::fmod(t, areaWidth) * division;
	t_2 = std::clamp(t_2, 0.0f, 1.0f);

	size_t index = static_cast< size_t >(t / areaWidth);
	index = std::clamp(int(index), 0, int(division - 1));

	// 4つの制御点のインデックス
	size_t index0 = index - 1;
	size_t index1 = index;
	size_t index2 = index + 1;
	size_t index3 = index + 2;


	//最初の区間のp0はp1を重複使用する
	if (index == 0){
		index0 = index1;
	}
	//最後の区間のp3はp2を重複使用する
	if (index3 >= points.size()){
		index3 = index2;
	}

	// インデックスが範囲を超えないようにする
	index0 = std::clamp(index0, size_t(0), points.size() - 1);
	index1 = std::clamp(index1, size_t(0), points.size() - 1);
	index2 = std::clamp(index2, size_t(0), points.size() - 1);
	index3 = std::clamp(index3, size_t(0), points.size() - 1);

	const Vector3& p0 = points[index0];
	const Vector3& p1 = points[index1];
	const Vector3& p2 = points[index2];
	const Vector3& p3 = points[index3];

	return CatmullRomInterpolation(p0, p1, p2, p3, t_2);
}
