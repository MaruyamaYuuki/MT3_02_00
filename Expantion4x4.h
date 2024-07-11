#pragma once
#include <cmath>
#include "Vector3.h"
#include "Matrix4x4.h"

class Expantion4x4 {
public:

    Matrix4x4 MakeIdentity4x4();
    double cot(double angle) {
        return 1.0 / std::tan(angle);
    }

    Matrix4x4 MakeTranslateMatrix(const Vector3& translate);
    Matrix4x4 MakeScaleMatrix(const Vector3& scale);
    Matrix4x4 MakeRotateXMatrix(float radius);
    Matrix4x4 MakeRotateYMatrix(float radius);
    Matrix4x4 MakeRotateZMatrix(float radius);
    Matrix4x4 Multiply(const Matrix4x4& m1, const Matrix4x4& m2);
    Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate);

    Vector3 Transform(const Vector3& vector, const Matrix4x4& matrix);
    Matrix4x4 MakePrespectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip);
    Matrix4x4 Inverse(const Matrix4x4& m);
    Matrix4x4 MakeViewportMatrix(float left, float top, float width, float height, float minDepth, float maxDepth);

    Vector3 GetTranslate(const Matrix4x4& m);
};
