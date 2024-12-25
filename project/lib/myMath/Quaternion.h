#pragma once

#include "Vector3.h"
#include "Matrix4x4.h"
#include <stdexcept>

struct Quaternion{
    float x; //< X成分
    float y; //< Y成分
    float z; //< Z成分
    float w; //< W成分

    // 単位クォータニオンで初期化する
    void Initialize();

    // 2つのクォータニオンのドット積（内積）を求める
    static float Dot(const Quaternion& q1, const Quaternion& q2);

    // ========================== 演算子オーバーロード ==========================

    // クォータニオン同士の加算
    Quaternion operator+(const Quaternion& q) const;

    // クォータニオン同士の減算
    Quaternion operator-(const Quaternion& q) const;

    // クォータニオン同士の乗算
    Quaternion operator*(const Quaternion& q) const;

    // クォータニオン同士の除算
    Quaternion operator/(const Quaternion& q) const;

    // ========================== 静的関数 ==========================

    // 球面線形補間(SLERP)を行う
    static Quaternion Slerp(const Quaternion& q0, const Quaternion& q1, float t);

    // 単位クォータニオンを生成する
    static Quaternion MakeIdentity();

    // クォータニオンの共役を求める
    static Quaternion Conjugate(const Quaternion& q);

    // クォータニオンのノルム(大きさ)を求める
    static float Norm(const Quaternion& q);

    // クォータニオンを正規化する
    static Quaternion Normalize(const Quaternion& q);

    // クォータニオンの逆数(逆クォータニオン)を求める
    static Quaternion Inverse(const Quaternion& q);

    // 任意軸回転を表すクォータニオンを生成する
    static Quaternion MakeRotateAxisQuaternion(const Vector3& axis, float angle);

    // クォータニオンから回転行列を生成する
    static Matrix4x4 ToMatrix(const Quaternion& q);

    // クォータニオンでベクトルを回転させる
    static Vector3 RotateVector(const Vector3& vector, const Quaternion& quaternion);

    // 2つのクォータニオンの積を計算する
    static Quaternion Multiply(const Quaternion& lhs, const Quaternion& rhs);
};
