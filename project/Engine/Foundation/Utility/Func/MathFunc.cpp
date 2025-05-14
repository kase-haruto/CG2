#include <Engine/Foundation/Utility/Func/MathFunc.h>
#include <Engine/Foundation/Math/Vector3.h>
/* c++ */
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
    assert(points.size() >= 4 && "制御点は最低4つ必要です。");

    size_t division = points.size() - 1;
    float areaWidth = 1.0f / division;

    // tに基づきどのセグメントにいるかを決定し、局所的なtを計算
    size_t index = std::clamp(static_cast<size_t>(t / areaWidth), size_t(0), division - 1);
    float t_2 = (t - index * areaWidth) * division;
    t_2 = std::clamp(t_2, 0.0f, 1.0f);

    // 制御点のインデックスをクランプして範囲内に収める
    size_t index0 = (index == 0) ? index : index - 1;
    size_t index1 = index;
    size_t index2 = std::min(index + 1, division);
    size_t index3 = std::min(index + 2, division);

    const Vector3& p0 = points[index0];
    const Vector3& p1 = points[index1];
    const Vector3& p2 = points[index2];
    const Vector3& p3 = points[index3];

    return CatmullRomInterpolation(p0, p1, p2, p3, t_2);
}