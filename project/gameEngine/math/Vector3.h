#pragma once
#include <cmath>

class Vector3 {
public:
	float x;
	float y;
	float z;

	Vector3(float x_ = 0, float y_ = 0, float z_ = 0) : x(x_), y(y_), z(z_) {}

	// 加算
	Vector3 operator+(const Vector3& obj) const;
	// 減算
	Vector3 operator-(const Vector3& obj) const;
	// 乗算
	Vector3 operator*(const Vector3& obj) const;
	// 乗算(スカラー倍)
	Vector3 operator*(const float& scalar) const;
	// 除算
	Vector3 operator/(const Vector3& obj) const;
	// +=
	Vector3& operator+=(const Vector3& obj);
	// -=
	Vector3& operator-=(const Vector3& obj);
	// *=
	Vector3& operator*=(const Vector3& obj);
	// /=
	Vector3& operator/=(const Vector3& obj);
};