#include "Vector3.h"

// 加算
Vector3 Vector3::operator+(const Vector3& obj) const { return Vector3(x + obj.x, y + obj.y, z + obj.z); }

// 減算
Vector3 Vector3::operator-(const Vector3& obj) const { return Vector3(x - obj.x, y - obj.y, z - obj.z); }

// 乗算
Vector3 Vector3::operator*(const Vector3& obj) const { return Vector3(x * obj.x, y * obj.y, z * obj.z); }

// 乗算(スカラー倍)
Vector3 Vector3::operator*(const float& scalar) const { return Vector3(x * scalar, y * scalar, z * scalar); }

// 除算
Vector3 Vector3::operator/(const Vector3& obj) const { return Vector3(x / obj.x, y / obj.y, z / obj.z); }

// +=
Vector3& Vector3::operator+=(const Vector3& other) {
	x += other.x;
	y += other.y;
	z += other.z;
	return *this;
}

// -=
Vector3& Vector3::operator-=(const Vector3& other) {
	x -= other.x;
	y -= other.y;
	z -= other.z;
	return *this;
}

// *=
Vector3& Vector3::operator*=(const Vector3& other) {
	x *= other.x;
	y *= other.y;
	z *= other.z;
	return *this;
}

// /=
Vector3& Vector3::operator/=(const Vector3& other) {
	x /= other.x;
	y /= other.y;
	z /= other.z;
	return *this;
}

