#include "Quaternion.h"
#include <cmath>
#include <cfloat>
#include <numbers>

///////////////////////////////////////////////////////////////////////////
//              メンバ関数
///////////////////////////////////////////////////////////////////////////

void Quaternion::Initialize(){
    // 自分自身を単位クォータニオンにする
    *this = MakeIdentity();
}

///////////////////////////////////////////////////////////////////////////
//              スタティック関数
///////////////////////////////////////////////////////////////////////////

float Quaternion::Dot(const Quaternion& q1, const Quaternion& q2){
    return q1.x * q2.x + q1.y * q2.y + q1.z * q2.z + q1.w * q2.w;
}

Quaternion Quaternion::MakeIdentity(){
    return {0.0f, 0.0f, 0.0f, 1.0f};
}

Quaternion Quaternion::Slerp(const Quaternion& q0, const Quaternion& q1, float t){
    float dot = Dot(q0, q1);
    Quaternion q1Mod = q1;

    // 内積が負の場合、q1を反転して短い経路を取る
    if (dot < 0.0f){
        q1Mod.x = -q1.x;
        q1Mod.y = -q1.y;
        q1Mod.z = -q1.z;
        q1Mod.w = -q1.w;
        dot = -dot;
    }

    // クォータニオンがほぼ同じ場合は線形補間
    if (dot > 1.0f - FLT_EPSILON){
        Quaternion result;
        result.x = (1.0f - t) * q0.x + t * q1Mod.x;
        result.y = (1.0f - t) * q0.y + t * q1Mod.y;
        result.z = (1.0f - t) * q0.z + t * q1Mod.z;
        result.w = (1.0f - t) * q0.w + t * q1Mod.w;
        return Normalize(result);
    }

    // なす角と補間係数を計算
    float theta = std::acos(dot);
    float sinTheta = std::sin(theta);

    float scale0 = std::sin((1.0f - t) * theta) / sinTheta;
    float scale1 = std::sin(t * theta) / sinTheta;

    Quaternion result;
    result.x = scale0 * q0.x + scale1 * q1Mod.x;
    result.y = scale0 * q0.y + scale1 * q1Mod.y;
    result.z = scale0 * q0.z + scale1 * q1Mod.z;
    result.w = scale0 * q0.w + scale1 * q1Mod.w;
    return Normalize(result);
}

Quaternion Quaternion::Conjugate(const Quaternion& q){
    return {-q.x, -q.y, -q.z, q.w};
}

float Quaternion::Norm(const Quaternion& q){
    return std::sqrt(q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w);
}

Quaternion Quaternion::Normalize(const Quaternion& q){
    float normQ = Norm(q);
    if (normQ == 0.0f){
        throw std::runtime_error("Cannot normalize a quaternion with zero norm.");
    }
    float inv = 1.0f / normQ;
    return {q.x * inv, q.y * inv, q.z * inv, q.w * inv};
}

Quaternion Quaternion::Inverse(const Quaternion& q){
    Quaternion conjugate = Conjugate(q);
    float normSq = Norm(q);
    normSq = normSq * normSq; // Norm の 2乗

    if (normSq == 0.0f){
        throw std::runtime_error("Cannot invert a quaternion with zero norm.");
    }
    return {conjugate.x / normSq,
            conjugate.y / normSq,
            conjugate.z / normSq,
            conjugate.w / normSq};
}

Quaternion Quaternion::MakeRotateAxisQuaternion(const Vector3& axis, float angle){
    Vector3 normalizedAxis = axis.Normalize();
    float halfAngle = angle * 0.5f;
    float sinHalfAngle = std::sin(halfAngle);

    return {
        normalizedAxis.x * sinHalfAngle,
        normalizedAxis.y * sinHalfAngle,
        normalizedAxis.z * sinHalfAngle,
        std::cos(halfAngle)
    };
}

Matrix4x4 Quaternion::ToMatrix(const Quaternion& q){
    float xx = q.x * q.x;
    float yy = q.y * q.y;
    float zz = q.z * q.z;
    float ww = q.w * q.w;
    float xy = q.x * q.y;
    float xz = q.x * q.z;
    float yz = q.y * q.z;
    float wx = q.w * q.x;
    float wy = q.w * q.y;
    float wz = q.w * q.z;

    return Matrix4x4({
        { ww + xx - yy - zz,   2.0f * (xy + wz),     2.0f * (xz - wy),     0.0f },
        { 2.0f * (xy - wz),    ww - xx + yy - zz,    2.0f * (yz + wx),     0.0f },
        { 2.0f * (xz + wy),    2.0f * (yz - wx),     ww - xx - yy + zz,    0.0f },
        { 0.0f,                0.0f,                 0.0f,                 1.0f }
                     });
}

Vector3 Quaternion::RotateVector(const Vector3& vector, const Quaternion& quaternion){
    Quaternion qVector {vector.x, vector.y, vector.z, 0.0f};
    Quaternion qConjugate = Conjugate(quaternion);
    Quaternion qResult = Multiply(Multiply(quaternion, qVector), qConjugate);

    return {qResult.x, qResult.y, qResult.z};
}

Quaternion Quaternion::Multiply(const Quaternion& lhs, const Quaternion& rhs){
    return lhs * rhs;
}

Vector3 Quaternion::ToEuler(const Quaternion& q){
    Vector3 euler;

    // roll (X軸回転)
    {
        double sinr_cosp = 2.0 * (q.w * q.x + q.y * q.z);
        double cosr_cosp = 1.0 - 2.0 * (q.x * q.x + q.y * q.y);
        euler.x = static_cast< float >(std::atan2(sinr_cosp, cosr_cosp));
    }

    // pitch (Y軸回転)
    {
        double sinp = 2.0 * (q.w * q.y - q.z * q.x);
        if (std::abs(sinp) >= 1.0){
            // -90° または +90° にクランプ
            euler.y = static_cast< float >(std::copysign(float(std::numbers::pi) / 2.0, sinp));
        } else{
            euler.y = static_cast< float >(std::asin(sinp));
        }
    }

    // yaw (Z軸回転)
    {
        double siny_cosp = 2.0 * (q.w * q.z + q.x * q.y);
        double cosy_cosp = 1.0 - 2.0 * (q.y * q.y + q.z * q.z);
        euler.z = static_cast< float >(std::atan2(siny_cosp, cosy_cosp));
    }

    return euler;
}

///////////////////////////////////////////////////////////////////////////
//              演算子オーバーロード
///////////////////////////////////////////////////////////////////////////

Quaternion Quaternion::operator+(const Quaternion& q) const{
    return {x + q.x, y + q.y, z + q.z, w + q.w};
}

Quaternion Quaternion::operator-(const Quaternion& q) const{
    return {x - q.x, y - q.y, z - q.z, w - q.w};
}

Quaternion Quaternion::operator*(const Quaternion& other) const{
    return {
        w * other.x + x * other.w + y * other.z - z * other.y,
        w * other.y - x * other.z + y * other.w + z * other.x,
        w * other.z + x * other.y - y * other.x + z * other.w,
        w * other.w - x * other.x - y * other.y - z * other.z
    };
}

Quaternion Quaternion::operator/(const Quaternion& q) const{
    float normSquared = q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w;
    if (normSquared == 0.0f){
        throw std::runtime_error("Cannot divide by a quaternion with zero norm.");
    }
    // q の共役をかけた後、ノルムの2乗で割る
    Quaternion conjugateQ = Conjugate(q);
    Quaternion multiplied = (*this) * conjugateQ;

    return {
        multiplied.x / normSquared,
        multiplied.y / normSquared,
        multiplied.z / normSquared,
        multiplied.w / normSquared
    };
}
