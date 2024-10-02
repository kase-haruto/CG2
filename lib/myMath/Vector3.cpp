#include"Vector3.h"
#include<cmath>
#include"Matrix4x4.h"
#include"myfunc/MyFunc.h"
#include <algorithm>

float Vector3::Length()const{
	return sqrtf(x * x + y * y + z * z);
}

Vector3 Vector3::Normalize()const{
	float length = Length();
	return Vector3(x / length, y / length, z / length);
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

Vector3 operator-(float scalar, const Vector3& vec){
	return Vector3(vec.x - scalar, vec.y - scalar, vec.z - scalar);
}