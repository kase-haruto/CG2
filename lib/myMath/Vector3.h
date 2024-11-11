#pragma once

/// <summary>
/// 3次元ベクトル
/// </summary>
struct Vector3 final{
    float x;
    float y;
    float z;

    float Length()const;
    float Dot(const Vector3& v);
    Vector3 Normalize()const;

    static Vector3 Cross(const Vector3& v1, const Vector3& v2){
        return {v1.y * v2.z - v1.z * v2.y,
                v1.z * v2.x - v1.x * v2.z,
                v1.x * v2.y - v1.y * v2.x};
    }

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