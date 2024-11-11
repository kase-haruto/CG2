#pragma once
#include "Vector3.h"

/// <summary>3
/// 4x4行列
/// </summary>
struct Matrix4x4 final{
	float m[4][4];

    // 右方向のベクトルを設定
    void SetRight(const Vector3& right){
        m[0][0] = right.x;
        m[1][0] = right.y;
        m[2][0] = right.z;
    }

    // 上方向のベクトルを設定
    void SetUp(const Vector3& up){
        m[0][1] = up.x;
        m[1][1] = up.y;
        m[2][1] = up.z;
    }

    // 前方向のベクトルを設定
    void SetForward(const Vector3& forward){
        m[0][2] = forward.x;
        m[1][2] = forward.y;
        m[2][2] = forward.z;
    }

    void SetColumn(int index, const Vector3& vec){
        if (index < 0 || index > 3){
            // 無効なインデックスの場合はエラー処理を行う
            return;
        }

        // 指定された列にベクトルの値を設定
        m[0][index] = vec.x;
        m[1][index] = vec.y;
        m[2][index] = vec.z;
        m[3][index] = 0.0f; // 通常、3x3の回転行列部分ではこの値は0になる
    }

    static Matrix4x4 FromBasis(const Vector3& right, const Vector3& up, const Vector3& forward);

    static Matrix4x4 CreateRotationMatrix(const Vector3& right, const Vector3& up, const Vector3& forward);

    // オイラー角を取得（ピッチ、ヨー、ロール）
    Vector3 ToEulerAngles();
    static Vector3 MatrixToEuler(const Matrix4x4& mat);

	static Matrix4x4 Multiply(const Matrix4x4& m1, const Matrix4x4& m2);
	static Matrix4x4 MakeIdentity();
	// 逆行列
	static Matrix4x4 Inverse(const Matrix4x4& m);

	static Vector3 Transform(const Vector3& vector, const Matrix4x4& matrix);

    // LookAtDirection関数の実装
    static Vector3 LookAtDirection(const Vector3& position, const Vector3& target);

	static Matrix4x4 MakeViewportMatrix(float l, float t, float w, float h, float minDepth, float maxDepth);
};
