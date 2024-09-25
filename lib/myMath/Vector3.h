#pragma once

/// <summary>
/// 3次元ベクトル
/// </summary>
struct Vector3 final{
    float x;
    float y;
    float z;

    float Length()const;
    Vector3 Normalize()const;

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
};