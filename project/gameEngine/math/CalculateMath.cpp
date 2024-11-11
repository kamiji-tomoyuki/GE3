#pragma once
#include "CalculateMath.h"

// 内積
float Dot(const Vector3& v1, const Vector3& v2) {
	float result;
	result = v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
	return result;
}
// 長さ
float Length(const Vector3& v) {
	float result;
	result = sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
	return result;
}
// 正規化 v/||v||
Vector3 Normalize(const Vector3& v) {
	Vector3 result;
	result.x = v.x / sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
	result.y = v.y / sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
	result.z = v.z / sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
	return result;
}
// クロス積
Vector3 Cross(const Vector3& v1, const Vector3& v2) {
	Vector3 result;
	result = {v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.x};

	return result;
}
// ベクトル変換
Vector3 TransformNormal(const Vector3& v, const Matrix4x4& m) {
	Vector3 result{
	    v.x * m.m[0][0] + v.y * m.m[1][0] + v.z * m.m[2][0],
	    v.x * m.m[0][1] + v.y * m.m[1][1] + v.z * m.m[2][1],
	    v.x * m.m[0][2] + v.y * m.m[1][2] + v.z * m.m[2][2],
	};

	return result;
}

// -----行列-----
// 行列の積
Matrix4x4 Multiply(const Matrix4x4& m1, const Matrix4x4& m2) {
	Matrix4x4 result;

	for (int row = 0; row < 4; row++) {
		for (int column = 0; column < 4; column++) {
			result.m[row][column] = m1.m[row][0] * m2.m[0][column] + m1.m[row][1] * m2.m[1][column] + m1.m[row][2] * m2.m[2][column] + m1.m[row][3] * m2.m[3][column];
		}
	}
	return result;
}

// 平行移動行列
Matrix4x4 MakeTranslateMatrix(const Vector3& translate) {
	Matrix4x4 result;

	result.m[0][0] = 1;
	result.m[0][1] = 0;
	result.m[0][2] = 0;
	result.m[0][3] = 0;

	result.m[1][0] = 0;
	result.m[1][1] = 1;
	result.m[1][2] = 0;
	result.m[1][3] = 0;

	result.m[2][0] = 0;
	result.m[2][1] = 0;
	result.m[2][2] = 1;
	result.m[2][3] = 0;

	result.m[3][0] = translate.x;
	result.m[3][1] = translate.y;
	result.m[3][2] = translate.z;
	result.m[3][3] = 1;

	return result;
}
// 拡大縮小行列
Matrix4x4 MakeScaleMatrix(const Vector3& scale) {
	Matrix4x4 result;

	result.m[0][0] = scale.x;
	result.m[0][1] = 0;
	result.m[0][2] = 0;
	result.m[0][3] = 0;

	result.m[1][0] = 0;
	result.m[1][1] = scale.y;
	result.m[1][2] = 0;
	result.m[1][3] = 0;

	result.m[2][0] = 0;
	result.m[2][1] = 0;
	result.m[2][2] = scale.z;
	result.m[2][3] = 0;

	result.m[3][0] = 0;
	result.m[3][1] = 0;
	result.m[3][2] = 0;
	result.m[3][3] = 1;

	return result;
}
// 回転行列
Matrix4x4 MakeRotateXMatrix(float radian) {
	Matrix4x4 result;
	result = {
	    1.0f, 0.0f, 0.0f, 0.0f, 0.0f, std::cos(radian), std::sin(radian), 0.0f, 0.0f, -std::sin(radian), std::cos(radian), 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
	};

	return result;
}
Matrix4x4 MakeRotateYMatrix(float radian) {
	Matrix4x4 result;
	result = {
	    std::cos(radian), 0.0f, -std::sin(radian), 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, std::sin(radian), 0.0f, std::cos(radian), 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
	};

	return result;
}
Matrix4x4 MakeRotateZMatrix(float radian) {
	Matrix4x4 result;
	result = {
	    std::cos(radian), std::sin(radian), 0.0f, 0.0f, -std::sin(radian), std::cos(radian), 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
	};

	return result;
}
// アフィン変換行列
Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate) {
	Matrix4x4 result;
	Matrix4x4 rotateXMatrix = MakeRotateXMatrix(rotate.x);
	Matrix4x4 rotateYMatrix = MakeRotateYMatrix(rotate.y);
	Matrix4x4 rotateZMatrix = MakeRotateZMatrix(rotate.z);
	Matrix4x4 rotateXYZMatrix = Multiply(rotateXMatrix, Multiply(rotateYMatrix, rotateZMatrix)); // R

	result = {
	    scale.x * rotateXYZMatrix.m[0][0],
	    scale.x * rotateXYZMatrix.m[0][1],
	    scale.x * rotateXYZMatrix.m[0][2],
	    0.0f,
	    scale.y * rotateXYZMatrix.m[1][0],
	    scale.y * rotateXYZMatrix.m[1][1],
	    scale.y * rotateXYZMatrix.m[1][2],
	    0.0f,
	    scale.z * rotateXYZMatrix.m[2][0],
	    scale.z * rotateXYZMatrix.m[2][1],
	    scale.z * rotateXYZMatrix.m[2][2],
	    0.0f,
	    translate.x,
	    translate.y,
	    translate.z,
	    1.0f};

	return result;
}

// 逆行列
Matrix4x4 Inverse(const Matrix4x4& m) {
	Matrix4x4 result;
	float a;
	Matrix4x4 b;

	a = 1 /
	    (m.m[0][0] * m.m[1][1] * m.m[2][2] * m.m[3][3] + m.m[0][0] * m.m[1][2] * m.m[2][3] * m.m[3][1] + m.m[0][0] * m.m[1][3] * m.m[2][1] * m.m[3][2] - m.m[0][0] * m.m[1][3] * m.m[2][2] * m.m[3][1] -
	     m.m[0][0] * m.m[1][2] * m.m[2][1] * m.m[3][3] - m.m[0][0] * m.m[1][1] * m.m[2][3] * m.m[3][2] - m.m[0][1] * m.m[1][0] * m.m[2][2] * m.m[3][3] - m.m[0][2] * m.m[1][0] * m.m[2][3] * m.m[3][1] -
	     m.m[0][3] * m.m[1][0] * m.m[2][1] * m.m[3][2] + m.m[0][3] * m.m[1][0] * m.m[2][2] * m.m[3][1] + m.m[0][2] * m.m[1][0] * m.m[2][1] * m.m[3][3] + m.m[0][1] * m.m[1][0] * m.m[2][3] * m.m[3][2] +
	     m.m[0][1] * m.m[1][2] * m.m[2][0] * m.m[3][3] + m.m[0][2] * m.m[1][3] * m.m[2][0] * m.m[3][1] + m.m[0][3] * m.m[1][1] * m.m[2][0] * m.m[3][2] - m.m[0][3] * m.m[1][2] * m.m[2][0] * m.m[3][1] -
	     m.m[0][2] * m.m[1][1] * m.m[2][0] * m.m[3][3] - m.m[0][1] * m.m[1][3] * m.m[2][0] * m.m[3][2] - m.m[0][1] * m.m[1][2] * m.m[2][3] * m.m[3][0] - m.m[0][2] * m.m[1][3] * m.m[2][1] * m.m[3][0] -
	     m.m[0][3] * m.m[1][1] * m.m[2][2] * m.m[3][0] + m.m[0][3] * m.m[1][2] * m.m[2][1] * m.m[3][0] + m.m[0][2] * m.m[1][1] * m.m[2][3] * m.m[3][0] + m.m[0][1] * m.m[1][3] * m.m[2][2] * m.m[3][0]);

	b.m[0][0] = m.m[1][1] * m.m[2][2] * m.m[3][3] + m.m[1][2] * m.m[2][3] * m.m[3][1] + m.m[1][3] * m.m[2][1] * m.m[3][2] - m.m[1][3] * m.m[2][2] * m.m[3][1] - m.m[1][2] * m.m[2][1] * m.m[3][3] -
	            m.m[1][1] * m.m[2][3] * m.m[3][2];
	b.m[0][1] = -m.m[0][1] * m.m[2][2] * m.m[3][3] - m.m[0][2] * m.m[2][3] * m.m[3][1] - m.m[0][3] * m.m[2][1] * m.m[3][2] + m.m[0][3] * m.m[2][2] * m.m[3][1] + m.m[0][2] * m.m[2][1] * m.m[3][3] +
	            m.m[0][1] * m.m[2][3] * m.m[3][2];
	b.m[0][2] = m.m[0][1] * m.m[1][2] * m.m[3][3] + m.m[0][2] * m.m[1][3] * m.m[3][1] + m.m[0][3] * m.m[1][1] * m.m[3][2] - m.m[0][3] * m.m[1][2] * m.m[3][1] - m.m[0][2] * m.m[1][1] * m.m[3][3] -
	            m.m[0][1] * m.m[1][3] * m.m[3][2];
	b.m[0][3] = -m.m[0][1] * m.m[1][2] * m.m[2][3] - m.m[0][2] * m.m[1][3] * m.m[2][1] - m.m[0][3] * m.m[1][1] * m.m[2][2] + m.m[0][3] * m.m[1][2] * m.m[2][1] + m.m[0][2] * m.m[1][1] * m.m[2][3] +
	            m.m[0][1] * m.m[1][3] * m.m[2][2];

	b.m[1][0] = -m.m[1][0] * m.m[2][2] * m.m[3][3] - m.m[1][2] * m.m[2][3] * m.m[3][0] - m.m[1][3] * m.m[2][0] * m.m[3][2] + m.m[1][3] * m.m[2][2] * m.m[3][0] + m.m[1][2] * m.m[2][0] * m.m[3][3] +
	            m.m[1][0] * m.m[2][3] * m.m[3][2];
	b.m[1][1] = m.m[0][0] * m.m[2][2] * m.m[3][3] + m.m[0][2] * m.m[2][3] * m.m[3][0] + m.m[0][3] * m.m[2][0] * m.m[3][2] - m.m[0][3] * m.m[2][2] * m.m[3][0] - m.m[0][2] * m.m[2][0] * m.m[3][3] -
	            m.m[0][0] * m.m[2][3] * m.m[3][2];
	b.m[1][2] = -m.m[0][0] * m.m[1][2] * m.m[3][3] - m.m[0][2] * m.m[1][3] * m.m[3][0] - m.m[0][3] * m.m[1][0] * m.m[3][2] + m.m[0][3] * m.m[1][2] * m.m[3][0] + m.m[0][2] * m.m[1][0] * m.m[3][3] +
	            m.m[0][0] * m.m[1][3] * m.m[3][2];
	b.m[1][3] = +m.m[0][0] * m.m[1][2] * m.m[2][3] + m.m[0][2] * m.m[1][3] * m.m[2][0] + m.m[0][3] * m.m[1][0] * m.m[2][2] - m.m[0][3] * m.m[1][2] * m.m[2][0] - m.m[0][2] * m.m[1][0] * m.m[2][3] -
	            m.m[0][0] * m.m[1][3] * m.m[2][2];

	b.m[2][0] = m.m[1][0] * m.m[2][1] * m.m[3][3] + m.m[1][1] * m.m[2][3] * m.m[3][0] + m.m[1][3] * m.m[2][0] * m.m[3][1] - m.m[1][3] * m.m[2][1] * m.m[3][0] - m.m[1][1] * m.m[2][0] * m.m[3][3] -
	            m.m[1][0] * m.m[2][3] * m.m[3][1];
	b.m[2][1] = -m.m[0][0] * m.m[2][1] * m.m[3][3] - m.m[0][1] * m.m[2][3] * m.m[3][0] - m.m[0][3] * m.m[2][0] * m.m[3][1] + m.m[0][3] * m.m[2][1] * m.m[3][0] + m.m[0][1] * m.m[2][0] * m.m[3][3] +
	            m.m[0][0] * m.m[2][3] * m.m[3][1];
	b.m[2][2] = m.m[0][0] * m.m[1][1] * m.m[3][3] + m.m[0][1] * m.m[1][3] * m.m[3][0] + m.m[0][3] * m.m[1][0] * m.m[3][1] - m.m[0][3] * m.m[1][1] * m.m[3][0] - m.m[0][1] * m.m[1][0] * m.m[3][3] -
	            m.m[0][0] * m.m[1][3] * m.m[3][1];
	b.m[2][3] = -m.m[0][0] * m.m[1][1] * m.m[2][3] - m.m[0][1] * m.m[1][3] * m.m[2][0] - m.m[0][3] * m.m[1][0] * m.m[2][1] + m.m[0][3] * m.m[1][1] * m.m[2][0] + m.m[0][1] * m.m[1][0] * m.m[2][3] +
	            m.m[0][0] * m.m[1][3] * m.m[2][1];

	b.m[3][0] = -m.m[1][0] * m.m[2][1] * m.m[3][2] - m.m[1][1] * m.m[2][2] * m.m[3][0] - m.m[1][2] * m.m[2][0] * m.m[3][1] + m.m[1][2] * m.m[2][1] * m.m[3][0] + m.m[1][1] * m.m[2][0] * m.m[3][2] +
	            m.m[1][0] * m.m[2][2] * m.m[3][1];
	b.m[3][1] = +m.m[0][0] * m.m[2][1] * m.m[3][2] + m.m[0][1] * m.m[2][2] * m.m[3][0] + m.m[0][2] * m.m[2][0] * m.m[3][1] - m.m[0][2] * m.m[2][1] * m.m[3][0] - m.m[0][1] * m.m[2][0] * m.m[3][2] -
	            m.m[0][0] * m.m[2][2] * m.m[3][1];
	b.m[3][2] = -m.m[0][0] * m.m[1][1] * m.m[3][2] - m.m[0][1] * m.m[1][2] * m.m[3][0] - m.m[0][2] * m.m[1][0] * m.m[3][1] + m.m[0][2] * m.m[1][1] * m.m[3][0] + m.m[0][1] * m.m[1][0] * m.m[3][2] +
	            m.m[0][0] * m.m[1][2] * m.m[3][1];
	b.m[3][3] = m.m[0][0] * m.m[1][1] * m.m[2][2] + m.m[0][1] * m.m[1][2] * m.m[2][0] + m.m[0][2] * m.m[1][0] * m.m[2][1] - m.m[0][2] * m.m[1][1] * m.m[2][0] - m.m[0][1] * m.m[1][0] * m.m[2][2] -
	            m.m[0][0] * m.m[1][2] * m.m[2][1];

	result.m[0][0] = a * b.m[0][0];
	result.m[0][1] = a * b.m[0][1];
	result.m[0][2] = a * b.m[0][2];
	result.m[0][3] = a * b.m[0][3];
	result.m[1][0] = a * b.m[1][0];
	result.m[1][1] = a * b.m[1][1];
	result.m[1][2] = a * b.m[1][2];
	result.m[1][3] = a * b.m[1][3];
	result.m[2][0] = a * b.m[2][0];
	result.m[2][1] = a * b.m[2][1];
	result.m[2][2] = a * b.m[2][2];
	result.m[2][3] = a * b.m[2][3];
	result.m[3][0] = a * b.m[3][0];
	result.m[3][1] = a * b.m[3][1];
	result.m[3][2] = a * b.m[3][2];
	result.m[3][3] = a * b.m[3][3];

	return result;
}
// 転置行列
Matrix4x4 Transpose(const Matrix4x4& m) {
	Matrix4x4 result;

	for (int row = 0; row < 4; row++) {
		for (int column = 0; column < 4; column++) {
			if (row == column) {
				result.m[row][column] = m.m[row][column];
			} else {
				result.m[row][column] = m.m[column][row];
			}
		}
	}
	return result;
}
// 単位行列の作成
Matrix4x4 MakeIdentity4x4() {
	Matrix4x4 result;

	for (int row = 0; row < 4; row++) {
		for (int column = 0; column < 4; column++) {
			if (row == column) {
				result.m[row][column] = 1;
			} else {
				result.m[row][column] = 0;
			}
		}
	}
	return result;
}

Matrix4x4 MakeOrthographicMatrix(float left, float top, float right, float bottom, float nearClip, float farClip) {
	return { 2.0f / (right - left),           0,   0, 0, 0, 2.0f / (top - bottom), 0, 0, 0, 0, 1.0f / (farClip - nearClip), 0, (left + right) / (left - right), (top + bottom) / (bottom - top),
			nearClip / (nearClip - farClip), 1.0f };
};
// vec * mat
Vector3 Multiply(const Vector3& vec, const Matrix4x4& mat) {
	Vector3 result;
	result.x = mat.m[0][0] * vec.x + mat.m[0][1] * vec.y + mat.m[0][2] * vec.z + mat.m[0][3] * 1.0f;
	result.y = mat.m[1][0] * vec.x + mat.m[1][1] * vec.y + mat.m[1][2] * vec.z + mat.m[1][3] * 1.0f;
	result.z = mat.m[2][0] * vec.x + mat.m[2][1] * vec.y + mat.m[2][2] * vec.z + mat.m[2][3] * 1.0f;
	// 念のため、w成分も計算して確認
	float w = mat.m[3][0] * vec.x + mat.m[3][1] * vec.y + mat.m[3][2] * vec.z + mat.m[3][3] * 1.0f;
	if (w != 1.0f) {
		result.x /= w;
		result.y /= w;
		result.z /= w;
	}
	return result;
}

// -----座標系-----
// 透視投影行列
Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip) {
	Matrix4x4 result;
	result = {
	    1 / aspectRatio * (1 / std::tan(fovY / 2)), 0.0f, 0.0f, 0.0f, 0.0f, 1 / std::tan(fovY / 2), 0.0f, 0.0f, 0.0f, 0.0f, farClip / (farClip - nearClip), 1.0f, 0.0f, 0.0f,
	    -nearClip * farClip / (farClip - nearClip), 0.0f,
	};

	return result;
}
// ビューポート変換行列
Matrix4x4 MakeViewportMatrix(float left, float top, float width, float height, float minDepth, float maxDepth) {
	Matrix4x4 result;
	result = {width / 2, 0.0f, 0.0f, 0.0f, 0.0f, -height / 2, 0.0f, 0.0f, 0.0f, 0.0f, maxDepth - minDepth, 0.0f, left + (width / 2), top + (height / 2), minDepth, 1.0f};

	return result;
}
// 座標変換
Vector3 Transform(const Vector3& vector, const Matrix4x4& matrix) {
	Vector3 result;
	result.x = vector.x * matrix.m[0][0] + vector.y * matrix.m[1][0] + vector.z * matrix.m[2][0] + 1.0f * matrix.m[3][0];
	result.y = vector.x * matrix.m[0][1] + vector.y * matrix.m[1][1] + vector.z * matrix.m[2][1] + 1.0f * matrix.m[3][1];
	result.z = vector.x * matrix.m[0][2] + vector.y * matrix.m[1][2] + vector.z * matrix.m[2][2] + 1.0f * matrix.m[3][2];
	float w = vector.x * matrix.m[0][3] + vector.y * matrix.m[1][3] + vector.z * matrix.m[2][3] + 1.0f * matrix.m[3][3];

	result.x /= w;
	result.y /= w;
	result.z /= w;
	return result;
}
