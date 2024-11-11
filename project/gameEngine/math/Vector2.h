#pragma once

/// <summary>
/// 2次元ベクトル
/// </summary>
#pragma once
#include <cmath> // sqrt関数を使用するため

/// <summary>
/// 2次元ベクトル
/// </summary>
struct Vector2 final {
	float x;
	float y;

	// コンストラクタ
	Vector2(float _x = 0.0f, float _y = 0.0f) : x(_x), y(_y) {}

	// 符号反転
	Vector2 operator-() const { return Vector2(-x, -y); }

	// 加算
	Vector2 operator+(const Vector2& obj) const { return Vector2(x + obj.x, y + obj.y); }
	// 減算
	Vector2 operator-(const Vector2& obj) const { return Vector2(x - obj.x, y - obj.y); }
	// 乗算
	Vector2 operator*(const Vector2& obj) const { return Vector2(x * obj.x, y * obj.y); }
	// 乗算(スカラー倍)(float型)
	Vector2 operator*(const float& scalar) const { return Vector2(x * scalar, y * scalar); }
	// 乗算(スカラー倍)(int型)
	Vector2 operator*(const int& scalar) const { return Vector2(x * scalar, y * scalar); }
	// 除算
	Vector2 operator/(const Vector2& obj) const { return Vector2(x / obj.x, y / obj.y); }
	// 除算(スカラー)(float型)
	Vector2 operator/(const float& scalar) const { return Vector2(x / scalar, y / scalar); }
	// 除算(スカラー)(int型)
	Vector2 operator/(const int& scalar) const { return Vector2(x / scalar, y / scalar); }

	// スカラー引き算
	Vector2 operator-(const float& scalar) const { return Vector2(x - scalar, y - scalar); }
	// スカラー引き算 (int型)
	Vector2 operator-(const int& scalar) const { return Vector2(x - scalar, y - scalar); }

	// フレンド関数: スカラー * ベクトル
	friend Vector2 operator*(const float& scalar, const Vector2& vec) { return vec * scalar; }
	// フレンド関数: スカラー / ベクトル
	friend Vector2 operator/(const float& scalar, const Vector2& vec) { return Vector2(scalar / vec.x, scalar / vec.y); }
	// フレンド関数: スカラー * ベクトル (int型)
	friend Vector2 operator*(const int& scalar, const Vector2& vec) { return vec * scalar; }
	// フレンド関数: スカラー / ベクトル (int型)
	friend Vector2 operator/(const int& scalar, const Vector2& vec) { return Vector2(scalar / vec.x, scalar / vec.y); }

	// Vector2 の == 演算子
	bool operator==(const Vector2& other) const { return x == other.x && y == other.y; }

	// +=
	Vector2& operator+=(const Vector2& other) {
		x += other.x;
		y += other.y;
		return *this;
	}
	// -=
	Vector2& operator-=(const Vector2& other) {
		x -= other.x;
		y -= other.y;
		return *this;
	}
	// *=
	Vector2& operator*=(const Vector2& other) {
		x *= other.x;
		y *= other.y;
		return *this;
	}
	// /=
	Vector2& operator/=(const Vector2& other) {
		x /= other.x;
		y /= other.y;
		return *this;
	}

	// スカラーに対する演算
	// += スカラー
	Vector2& operator+=(const float& s) {
		x += s;
		y += s;
		return *this;
	}
	// -= スカラー
	Vector2& operator-=(const float& s) {
		x -= s;
		y -= s;
		return *this;
	}
	// *= スカラー
	Vector2& operator*=(const float& s) {
		x *= s;
		y *= s;
		return *this;
	}
	// /= スカラー
	Vector2& operator/=(const float& s) {
		x /= s;
		y /= s;
		return *this;
	}

	// ベクトルの長さを計算
	float Length() const { return std::sqrt(x * x + y * y); }

	// ベクトルを正規化（単位ベクトルにする）
	Vector2 Normalize() const {
		float len = Length();
		// 長さが0の場合は正規化できないので、ゼロベクトルを返す
		if (len == 0.0f) {
			return Vector2(0.0f, 0.0f);
		}
		return Vector2(x / len, y / len);
	}
};

struct Vector2Int final {
	int x;
	int y;

	// 符号反転
	Vector2Int operator-() const { return Vector2Int(-x, -y); }

	// 加算
	Vector2Int operator+(const Vector2Int& obj) const { return Vector2Int(x + obj.x, y + obj.y); };
	// 減算
	Vector2Int operator-(const Vector2Int& obj) const { return Vector2Int(x - obj.x, y - obj.y); };
	// 乗算
	Vector2Int operator*(const Vector2Int& obj) const { return Vector2Int(x * obj.x, y * obj.y); };
	// 乗算(スカラー倍)(int型)
	Vector2Int operator*(const int& scalar) const { return Vector2Int(x * scalar, y * scalar); };
	// 除算
	Vector2Int operator/(const Vector2Int& obj) const { return Vector2Int(x / obj.x, y / obj.y); };
	// 除算(スカラー)(int型)
	Vector2Int operator/(const int& scalar) const { return Vector2Int(x / scalar, y / scalar); };

	friend Vector2Int operator*(const int& scalar, const Vector2Int& vec) { return vec * scalar; }
	friend Vector2Int operator/(const int& scalar, const Vector2Int& vec) { return vec / scalar; }

	// Vector2Int の == 演算子
	bool operator==(const Vector2Int& other) const { return x == other.x && y == other.y; }

	// +=
	Vector2Int& operator+=(const Vector2Int& other) {
		x += other.x;
		y += other.y;

		return *this;
	};
	// -=
	Vector2Int& operator-=(const Vector2Int& other) {
		x -= other.x;
		y -= other.y;

		return *this;
	};
	// *=
	Vector2Int& operator*=(const Vector2Int& other) {
		x *= other.x;
		y *= other.y;

		return *this;
	};
	// /=
	Vector2Int& operator/=(const Vector2Int& other) {
		x /= other.x;
		y /= other.y;
		return *this;
	};
};