#pragma once
#include "Matrix4x4.h"
#include "CalculateMath.h"
class Camera
{
public:
	// コンストラクタ
	Camera();

	// 更新処理
	void Update();

public:
	// RT
	const Vector3& GetRotate() const { return transform.rotate; }
	void SetRotate(Vector3 rotate) { this->transform.rotate = rotate; }
	const Vector3& GetTranslate() const { return transform.translate; }
	void SetTranslate(Vector3 translate) { this->transform.translate = translate; }

	// projectionMatrix
	void SetFovY(float fovY) { this->fovY = fovY; }
	void SetAspectRatio(float aspectRatio) { this->aspectRatio = aspectRatio; }
	void SetNearClip(float nearClip) { this->nearClip = nearClip; }
	void SetFarClip(float farClip) { this->farClip = farClip; }

	// Matrix
	const Matrix4x4& GetWorldMatrix() const { return worldMatrix; }
	const Matrix4x4& GetViewMatrix() const { return viewMatrix; }
	const Matrix4x4& GetProjectionMatrix() const { return projectionMatrix; }
	const Matrix4x4& GetViewProjectionMatrix() const { return viewProjectionMatrix; }

private:
	// --- Transform ---
	struct Transform {
		Vector3 scale;
		Vector3 rotate;
		Vector3 translate;
	};

	// --- ビュー行列関連データ ---
	Transform transform;
	Matrix4x4 worldMatrix;
	Matrix4x4 viewMatrix;

	// --- プロジェクション行列関連データ ---
	Matrix4x4 projectionMatrix;
	float fovY;			// 水平方向視野角
	float aspectRatio;	// アスペクト比
	float nearClip;		// ニアクリップ距離
	float farClip;		// ファークリップ距離

	// --- 合成行列 ---
	Matrix4x4 viewProjectionMatrix;
};

