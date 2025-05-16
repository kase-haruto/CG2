#include <Engine/Foundation/Math/Vector3.h>
/* ========================================================================
/* include space
/* ===================================================================== */
#include <Engine/Foundation/Math/Matrix4x4.h>
#include <Engine/Foundation/Math/Quaternion.h>
#include <Engine/Foundation/Utility/Func/MyFunc.h>

/* c++ */
#include <algorithm>
#include <cmath>

void Vector3::Initialize(const Vector3& value){
	//値で初期化
	x = value.x;
	y = value.y;
	z = value.z;
}

void Vector3::Initialize(float v){
	//値で初期化
	x = v;
	y = v;
	z = v;
}

float Vector3::Length()const{
	return sqrtf(x * x + y * y + z * z);
}

Vector3 Vector3::Normalize()const{
	float length = Length();
	return Vector3(x / length, y / length, z / length);
}

float Vector3::LengthSquared() const {
	return x * x + y * y + z * z;
}

Vector3 Vector3::Forward() {
	return Vector3(0.0f, 0.0f, 1.0f);
}

Vector3 Vector3::One() {
	return Vector3(1.0f, 1.0f, 1.0f);
}

Vector3 Vector3::Cross(const Vector3& v0, const Vector3& v1){
	return {
	  v0.y * v1.z - v0.z * v1.y,
	  v0.z * v1.x - v0.x * v1.z,
	  v0.x * v1.y - v0.y * v1.x
	};
}

float Vector3::Dot(const Vector3& v1, const Vector3& v2) {
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

const Vector3 Vector3::Zero(0.0f, 0.0f, 0.0f);

Vector3 Vector3::Lerp(const Vector3& v1, const Vector3& v2, float t){

	return Vector3(
		v1.x + t * (v2.x - v1.x),
		v1.y + t * (v2.y - v1.y),
		v1.z + t * (v2.z - v1.z)
	);
}

Vector3 Vector3::Transform(const Vector3& vector, const Matrix4x4& matrix) {
	Vector3 result = { 0, 0, 0 };

	// 同次座標系への変換
	// 変換行列を適用
	Vector4 homogeneousCoordinate(
		vector.x * matrix.m[0][0] + vector.y * matrix.m[1][0] + vector.z * matrix.m[2][0] + matrix.m[3][0],
		vector.x * matrix.m[0][1] + vector.y * matrix.m[1][1] + vector.z * matrix.m[2][1] + matrix.m[3][1],
		vector.x * matrix.m[0][2] + vector.y * matrix.m[1][2] + vector.z * matrix.m[2][2] + matrix.m[3][2],
		vector.x * matrix.m[0][3] + vector.y * matrix.m[1][3] + vector.z * matrix.m[2][3] + matrix.m[3][3]);

	// 同次座標系から3次元座標系に戻す
	float w = homogeneousCoordinate.w;
	result.x = homogeneousCoordinate.x / w;
	result.y = homogeneousCoordinate.y / w;
	result.z = homogeneousCoordinate.z / w;

	return result;
}

Vector3 Vector3::Transform(const Vector3& v, const Quaternion& q) {
	Quaternion normQ = Quaternion::Normalize(q); // 安全
	Vector3 u(normQ.x, normQ.y, normQ.z);
	float s = normQ.w;

	return
		u * (2.0f * Vector3::Dot(u, v)) +
		v * (s * s - Vector3::Dot(u, u)) +
		Vector3::Cross(u, v) * (2.0f * s);
}

//乗算
Vector3 Vector3::operator*(const float& scalar) const{
	float newX = x * scalar;
	float newY = y * scalar;
	float newZ = z * scalar;
	return Vector3(newX, newY, newZ);
}

Vector3 Vector3::operator*=(const float& scalar){
	x *= scalar;
	y *= scalar;
	z *= scalar;
	return Vector3(x, y, z);
}

Vector3 Vector3::operator*(const Vector3& other) const{
	float newX = x * other.x;
	float newY = y * other.y;
	float newZ = z * other.z;
	return Vector3(newX, newY, newZ);
}

Vector3 Vector3::operator*=(const Vector3& other){
	x *= other.x;
	y *= other.y;
	z *= other.z;
	return Vector3(x, y, z);
}

Vector3 operator*(float scalar, const Vector3& vec){
	return Vector3(vec.x * scalar, vec.y * scalar, vec.z * scalar);
}



//除算
Vector3 Vector3::operator/(const float& scalar) const{
	float newX = x / scalar;
	float newY = y / scalar;
	float newZ = z / scalar;
	return Vector3(newX, newY, newZ);
}

Vector3 Vector3::operator/=(const float& scalar){
	x /= scalar;
	y /= scalar;
	z /= scalar;
	return Vector3(x, y, z);
}

Vector3 Vector3::operator/(const Vector3& other) const{
	float newX = x / other.x;
	float newY = y / other.y;
	float newZ = z / other.z;
	return Vector3(newX, newY, newZ);
}

Vector3 Vector3::operator/=(const Vector3& other){
	x /= other.x;
	y /= other.y;
	z /= other.z;
	return Vector3(x, y, z);
}

Vector3 operator/(float scalar, const Vector3& vec){
	return Vector3(vec.x / scalar, vec.y / scalar, vec.z / scalar);
}


// ベクトルの加算
Vector3 Vector3::operator+(const float& scalar) const{
	float newX = x + scalar;
	float newY = y + scalar;
	float newZ = z + scalar;
	return Vector3(newX, newY, newZ);
}

Vector3 Vector3::operator+=(const float& scalar){
	x += scalar;
	y += scalar;
	z += scalar;
	return Vector3(x, y, z);
}

Vector3 Vector3::operator+(const Vector3& other) const{
	float newX = x + other.x;
	float newY = y + other.y;
	float newZ = z + other.z;
	return Vector3(newX, newY, newZ);
}

Vector3 Vector3::operator+=(const Vector3& other){
	x += other.x;
	y += other.y;
	z += other.z;
	return Vector3(x, y, z);
}

Vector3 operator+(float scalar, const Vector3& vec){
	return Vector3(vec.x + scalar, vec.y + scalar, vec.z + scalar);
}

// ベクトルの減算
Vector3 Vector3::operator-(const float& scalar) const{
	float newX = x - scalar;
	float newY = y - scalar;
	float newZ = z - scalar;
	return Vector3(newX, newY, newZ);
}

Vector3 Vector3::operator-=(const float& scalar){
	x -= scalar;
	y -= scalar;
	z -= scalar;
	return Vector3(x, y, z);
}

Vector3 Vector3::operator-(const Vector3& other) const{
	float newX = x - other.x;
	float newY = y - other.y;
	float newZ = z - other.z;
	return Vector3(newX, newY, newZ);
}

Vector3 Vector3::operator-=(const Vector3& other){
	x -= other.x;
	y -= other.y;
	z -= other.z;
	return Vector3(x, y, z);
}


Vector3 operator-(float scalar, const Vector3& vec) {
	return Vector3(vec.x - scalar, vec.y - scalar, vec.z - scalar);
}


