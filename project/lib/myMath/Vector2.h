#pragma once

struct Vector2{
	float x;
	float y;

	//* operator overloads
	Vector2 operator+(const Vector2& v) const{
		return {x + v.x, y + v.y};
	}

	Vector2 operator-(const Vector2& v) const{
		return {x - v.x, y - v.y};
	}
};