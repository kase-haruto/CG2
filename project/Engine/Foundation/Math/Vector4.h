#pragma once

#include <externals/nlohmann/json.hpp>

struct Matrix4x4;
struct Vector3;

struct Vector4 final{
	float x;
	float y;
	float z;
	float w;

	Vector4(const struct Vector3& v, float w = 1.0f);
	Vector4(float x = 0.0f, float y = 0.0f, float z = 0.0f, float w = 1.0f) :
		x(x), y(y), z(z), w(w) {}

	//--------- function ---------------------------------------------------
	static Vector4 TransformVector(const Matrix4x4& m, const Vector4& v);

	//--------- operator ---------------------------------------------------
	bool operator==(const Vector4& other) const;
	bool operator!=(const Vector4& other) const;
	Vector4 operator*(const float& scalar) const;
	Vector4 operator*=(const float& scalar);
	Vector4 operator*(const Vector4& other) const;
	Vector4 operator*=(const Vector4& other);
	Vector4 operator/(const float& scalar) const;
	Vector4 operator/=(const float& scalar);

	//--------- utility ---------------------------------------------------
	Vector3 xyz() const;
};

//--------- serializer ---------------------------------------------------
inline void to_json(nlohmann::json& j, const Vector4& v) {
	j = nlohmann::json{ {"x", v.x}, {"y", v.y}, {"z", v.z}, {"w", v.w} };
}
inline void from_json(const nlohmann::json& j, Vector4& v) {
	j.at("x").get_to(v.x);
	j.at("y").get_to(v.y);
	j.at("z").get_to(v.z);
	j.at("w").get_to(v.w);
}