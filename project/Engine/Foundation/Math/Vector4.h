#pragma once

#include <externals/nlohmann/json.hpp>

struct Matrix4x4;

struct Vector4 final{
	float x;
	float y;
	float z;
	float w;

	//--------- function ---------------------------------------------------
	static Vector4 TransformVector(const Matrix4x4& m, const Vector4& v);

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