#pragma once
#include <cstring>

class Matrix4x4 {
public:
	float m[4][4];

	// 加算
	Matrix4x4 operator+(const Matrix4x4& mat) const;

	// 減算
	Matrix4x4 operator-(const Matrix4x4& mat) const;

	// 乗算
	Matrix4x4 operator*(const Matrix4x4& mat) const;

	// 乗算(スカラー倍)
	Matrix4x4 operator*(const float& scalar) const;

	// 除算
	Matrix4x4 operator/(const float& scalar) const;

	// +=
	Matrix4x4& operator+=(const Matrix4x4& mat);

	// -=
	Matrix4x4& operator-=(const Matrix4x4& mat);

	// *=
	Matrix4x4& operator*=(const Matrix4x4& mat);

	// /=
	Matrix4x4& operator/=(const float& scalar);
};