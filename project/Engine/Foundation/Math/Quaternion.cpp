#include "Quaternion.h"
#include <cmath>
#include <cfloat>
#include <numbers>

///////////////////////////////////////////////////////////////////////////
//              メンバ関数
///////////////////////////////////////////////////////////////////////////

Quaternion::Quaternion(){
	// デフォルトコンストラクタ
	x = 0.0f;
	y = 0.0f;
	z = 0.0f;
	w = 1.0f; // 単位クォータニオン
}

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

Quaternion Quaternion::Slerp(Quaternion q0, const Quaternion& q1, float t){
	// q0とq1の内積
	float dot = Dot(q0, q1);

	if (dot < 0.0f){

		q0 = -q0;
		dot = -dot;
	}

	if (dot >= 1.0f - FLT_EPSILON){

		return (1.0f - t) * q0 + t * q1;
	}

	float theta = std::acos(dot);
	float sinTheta = std::sin(theta);

	float scale0 = std::sin((1.0f - t) * theta) / sinTheta;
	float scale1 = std::sin(t * theta) / sinTheta;

	// 補完後のクォータニオンを求める
	return q0 * scale0 + q1 * scale1;
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
		-normalizedAxis.x * sinHalfAngle,
		-normalizedAxis.y * sinHalfAngle,
		-normalizedAxis.z * sinHalfAngle,
		std::cos(halfAngle)
	};
}

Matrix4x4 Quaternion::ToMatrix(const Quaternion& quaternion){
	Matrix4x4 result;
	float xx = quaternion.x * quaternion.x;
	float yy = quaternion.y * quaternion.y;
	float zz = quaternion.z * quaternion.z;
	float ww = quaternion.w * quaternion.w;
	float xy = quaternion.x * quaternion.y;
	float xz = quaternion.x * quaternion.z;
	float yz = quaternion.y * quaternion.z;
	float wx = quaternion.w * quaternion.x;
	float wy = quaternion.w * quaternion.y;
	float wz = quaternion.w * quaternion.z;

	result.m[0][0] = ww + xx - yy - zz;
	result.m[0][1] = 2.0f * (xy + wz);
	result.m[0][2] = 2.0f * (xz - wy);
	result.m[0][3] = 0.0f;

	result.m[1][0] = 2.0f * (xy - wz);
	result.m[1][1] = ww - xx + yy - zz;
	result.m[1][2] = 2.0f * (yz + wx);
	result.m[1][3] = 0.0f;

	result.m[2][0] = 2.0f * (xz + wy);
	result.m[2][1] = 2.0f * (yz - wx);
	result.m[2][2] = ww - xx - yy + zz;
	result.m[2][3] = 0.0f;

	result.m[3][0] = 0.0f;
	result.m[3][1] = 0.0f;
	result.m[3][2] = 0.0f;
	result.m[3][3] = 1.0f;

	return result;

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

Quaternion Quaternion::EulerToQuaternion(const Vector3& euler){
	Quaternion qx = Quaternion::MakeRotateX(euler.x);
	Quaternion qy = Quaternion::MakeRotateY(euler.y);
	Quaternion qz = Quaternion::MakeRotateZ(euler.z);
	return qz * qy * qx;
}

Quaternion Quaternion::MakeRotateX(float radian){
	float half = radian * 0.5f;
	return {std::sin(half), 0.0f, 0.0f, std::cos(half)};
}

Quaternion Quaternion::MakeRotateY(float radian){
	float half = radian * 0.5f;
	return {0.0f, std::sin(half), 0.0f, std::cos(half)};
}

Quaternion Quaternion::MakeRotateZ(float radian){
	float half = radian * 0.5f;
	return {0.0f, 0.0f, std::sin(half), std::cos(half)};
}




Vector3 Quaternion::ToEuler(const Quaternion& q){
	Vector3 euler;

	const double xx = q.x * q.x;
	const double yy = q.y * q.y;
	const double zz = q.z * q.z;
	const double ww = q.w * q.w;

	// Roll (X軸回転)
	{
		double sinr_cosp = 2.0 * (q.w * q.x + q.y * q.z);
		double cosr_cosp = ww - yy - zz + xx;
		euler.x = static_cast< float >(std::atan2(sinr_cosp, cosr_cosp));
	}

	// Pitch (Y軸回転)
	{
		double sinp = 2.0 * (q.w * q.y - q.z * q.x);
		if (std::abs(sinp) >= 1.0){
			euler.y = static_cast< float >(std::copysign(std::numbers::pi_v<float> / 2.0f, sinp)); // ±90°
		} else{
			euler.y = static_cast< float >(std::asin(sinp));
		}
	}

	// Yaw (Z軸回転)
	{
		double siny_cosp = 2.0 * (q.w * q.z + q.x * q.y);
		double cosy_cosp = ww - xx - yy + zz;
		euler.z = static_cast< float >(std::atan2(siny_cosp, cosy_cosp));
	}

	return euler;
}


Quaternion Quaternion::FromToQuaternion(const Vector3& from, const Vector3& to){
	Vector3 fromNorm = from.Normalize();
	Vector3 toNorm = to.Normalize();

	float dot = Vector3::Dot(fromNorm, toNorm);
	Vector3 axis;

	if (dot > 0.9999f){
		return Quaternion::MakeIdentity();
	}

	if (dot < -0.9999f){
		// 180度回転
		axis = Vector3::Cross(Vector3(1, 0, 0), fromNorm);
		if (axis.LengthSquared() < 0.0001f){
			axis = Vector3::Cross(Vector3(0, 1, 0), fromNorm);
		}
		axis = axis.Normalize();
		return Quaternion::MakeRotateAxisQuaternion(axis, std::numbers::pi_v<float>);
	}

	// 左手座標系なので Cross(to, from)
	axis = Vector3::Cross(toNorm, fromNorm);
	float angle = std::acos(dot);
	return Quaternion::MakeRotateAxisQuaternion(axis, angle);
}


///////////////////////////////////////////////////////////////////////////
//              演算子オーバーロード
///////////////////////////////////////////////////////////////////////////

Quaternion Quaternion::operator+(const Quaternion& q) const{
	return {x + q.x, y + q.y, z + q.z, w + q.w};
}


Quaternion Quaternion::operator*(const Quaternion& other) const{
	return {
		w * other.x + x * other.w + y * other.z - z * other.y,
		w * other.y - x * other.z + y * other.w + z * other.x,
		w * other.z + x * other.y - y * other.x + z * other.w,
		w * other.w - x * other.x - y * other.y - z * other.z
	};
}

Quaternion Quaternion::operator-() const{
	return Quaternion {-x, -y, -z, -w};
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

Quaternion operator*(float scalar, const Quaternion& q){
	return {scalar * q.x,scalar * q.y ,scalar * q.z ,scalar * q.w};
}
Quaternion Quaternion::operator*(float scalar) const{
	return {x * scalar, y * scalar, z * scalar, w * scalar};
}


