#pragma once

#include "lib/myMath/Matrix4x4.h"

/// <summary>
/// 3次元ベクトル
/// </summary>
struct Vector3 final{
    float x;
    float y;
    float z;

    float Length()const;
    Vector3 Normalize()const;
    float LengthSquared() const{
        return x * x + y * y + z * z;
    }

    static Vector3 Cross(const Vector3& v0, const Vector3& v1);

    static float Dot(const Vector3& v1, const Vector3& v2){
        return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
    }

    static const Vector3 Zero;

    static Vector3 Lerp(const Vector3& v1, const Vector3& v2, float t);

    Vector3 operator*(const float& scalar) const;
    Vector3 operator*=(const float& scalar);
    Vector3 operator*(const Vector3& other) const;
    Vector3 operator*=(const Vector3& other);
    friend Vector3 operator*(float scalar, const Vector3& v);

    Vector3 operator/(const float& scalar) const;
    Vector3 operator/=(const float& scalar);
    Vector3 operator/(const Vector3& other)const;
    Vector3 operator/=(const Vector3& other);
    friend Vector3 operator/(float scalar, const Vector3& v);

    // ベクトルの加算
    Vector3 operator+(const float& scalar) const;
    Vector3 operator+=(const float& scalar);
    Vector3 operator+(const Vector3& other) const;
    Vector3 operator+=(const Vector3& other);
    friend Vector3 operator+(float scalar, const Vector3& v);

    // ベクトルの減算
    Vector3 operator-(const float& scalar) const;
    Vector3 operator-=(const float& scalar);
    Vector3 operator-(const Vector3& other) const;
    Vector3 operator-=(const Vector3& other);
    friend Vector3 operator-(float scalar, const Vector3& v);


    static Vector3 Transform(const Vector3& vector, const Matrix4x4& matrix);
};