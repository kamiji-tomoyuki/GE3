#include "Matrix4x4.h"

Matrix4x4 Matrix4x4::operator+(const Matrix4x4& mat) const {
	Matrix4x4 result;
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			result.m[i][j] = m[i][j] + mat.m[i][j];
		}
	}
	return result;
}

Matrix4x4 Matrix4x4::operator-(const Matrix4x4& mat) const {
	Matrix4x4 result;
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			result.m[i][j] = m[i][j] - mat.m[i][j];
		}
	}
	return result;
}

Matrix4x4 Matrix4x4::operator*(const Matrix4x4& mat) const {
	Matrix4x4 result;
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			result.m[i][j] = 0;
			for (int k = 0; k < 4; ++k) {
				result.m[i][j] += m[i][k] * mat.m[k][j];
			}
		}
	}
	return result;
}

Matrix4x4 Matrix4x4::operator*(const float& scalar) const {
	Matrix4x4 result;
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			result.m[i][j] = m[i][j] * scalar;
		}
	}
	return result;
}

Matrix4x4 Matrix4x4::operator/(const float& scalar) const {
	Matrix4x4 result;
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			result.m[i][j] = m[i][j] / scalar;
		}
	}
	return result;
}

Matrix4x4& Matrix4x4::operator+=(const Matrix4x4& mat) {
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			m[i][j] += mat.m[i][j];
		}
	}
	return *this;
}

Matrix4x4& Matrix4x4::operator-=(const Matrix4x4& mat) {
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			m[i][j] -= mat.m[i][j];
		}
	}
	return *this;
}

Matrix4x4& Matrix4x4::operator*=(const Matrix4x4& mat) {
	Matrix4x4 result = (*this) * mat;
	*this = result;
	return *this;
}

Matrix4x4& Matrix4x4::operator/=(const float& scalar) {
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			m[i][j] /= scalar;
		}
	}
	return *this;
}
