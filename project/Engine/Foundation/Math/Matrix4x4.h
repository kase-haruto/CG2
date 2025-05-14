#pragma once


struct Vector3;

/// <summary>
/// 4x4行列
/// </summary>
struct Matrix4x4 final{
	float m[4][4];

	static Matrix4x4 Multiply(const Matrix4x4& m1, const Matrix4x4& m2);
	static Matrix4x4 MakeIdentity();
	// 逆行列
	static Matrix4x4 Inverse(const Matrix4x4& m);

	static Vector3 ToEuler(const Matrix4x4& matrix);

	//転置
	static Matrix4x4 Transpose(const Matrix4x4& mat);

	static Vector3 Transform(const Vector3& vector, const Matrix4x4& matrix);
	static Matrix4x4 MakeLookRotationMatrix(const Vector3& forward, const Vector3& up);
	static Matrix4x4 MakeViewportMatrix(float left, float top, float width, float height, float minDepth, float maxDepth){

		Matrix4x4 matrix = {};

		for (int i = 0; i < 4; i++){
			for (int j = 0; j < 4; j++){
				matrix.m[i][j] = 0.0f;
			}
		}

		matrix.m[0][0] = width / 2.0f;
		matrix.m[1][1] = -height / 2.0f;
		matrix.m[2][2] = maxDepth - minDepth;
		matrix.m[3][0] = left + width / 2.0f;
		matrix.m[3][1] = top + height / 2.0f;
		matrix.m[3][2] = minDepth;
		matrix.m[3][3] = 1.0f;

		return matrix;
	}

	//operator関数
	Matrix4x4 operator*(const Matrix4x4& other) const{
		return Multiply(*this, other);
	}
};
