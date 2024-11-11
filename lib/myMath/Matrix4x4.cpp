#include"Matrix4x4.h"
#include"Vector4.h"

/*lib*/
#include <cassert>
#include <numbers>
#include<cmath>

float cot(float angle){
	return 1 / std::tan(angle);
}

Matrix4x4 Matrix4x4::FromBasis(const Vector3& right, const Vector3& up, const Vector3& forward){
	Matrix4x4 matrix;

	matrix.m[0][0] = right.x;
	matrix.m[1][0] = right.y;
	matrix.m[2][0] = right.z;
	matrix.m[3][0] = 0.0f;

	matrix.m[0][1] = up.x;
	matrix.m[1][1] = up.y;
	matrix.m[2][1] = up.z;
	matrix.m[3][1] = 0.0f;

	matrix.m[0][2] = forward.x;
	matrix.m[1][2] = forward.y;
	matrix.m[2][2] = forward.z;
	matrix.m[3][2] = 0.0f;

	matrix.m[0][3] = 0.0f;
	matrix.m[1][3] = 0.0f;
	matrix.m[2][3] = 0.0f;
	matrix.m[3][3] = 1.0f;

	return matrix;
}

Matrix4x4 Matrix4x4::CreateRotationMatrix(const Vector3& right, const Vector3& up, const Vector3& forward){
	Matrix4x4 mat;
	mat.m[0][0] = right.x;
	mat.m[1][0] = right.y;
	mat.m[2][0] = right.z;

	mat.m[0][1] = up.x;
	mat.m[1][1] = up.y;
	mat.m[2][1] = up.z;

	mat.m[0][2] = forward.x;
	mat.m[1][2] = forward.y;
	mat.m[2][2] = forward.z;

	// 同時に平行移動成分やスケール成分も設定できますが、ここでは回転行列のみの構築とします。
	return mat;
}



Vector3 Matrix4x4::MatrixToEuler(const Matrix4x4& mat){
	Vector3 euler;

	// ここではXYZの回転順を仮定しています。
	if (mat.m[0][2] < 1){
		if (mat.m[0][2] > -1){
			euler.y = asin(mat.m[0][2]);
			euler.x = atan2(-mat.m[1][2], mat.m[2][2]);
			euler.z = atan2(-mat.m[0][1], mat.m[0][0]);
		} else{
			euler.y = float(-std::numbers::pi) / 2;
			euler.x = -atan2(mat.m[1][0], mat.m[1][1]);
			euler.z = 0;
		}
	} else{
		euler.y = float(std::numbers::pi) / 2;
		euler.x = atan2(mat.m[1][0], mat.m[1][1]);
		euler.z = 0;
	}

	return euler;
}


Matrix4x4 Matrix4x4::Multiply(const Matrix4x4& m1, const Matrix4x4& m2){
	Matrix4x4 result;

	for (int i = 0; i < 4; ++i){
		for (int j = 0; j < 4; ++j){
			result.m[i][j] = 0.0f;
			for (int k = 0; k < 4; ++k){
				result.m[i][j] += m1.m[i][k] * m2.m[k][j];
			}
		}
	}

	return result;
}

Matrix4x4 Matrix4x4::MakeIdentity(){
	Matrix4x4 result;
	for (int i = 0; i < 4; ++i){
		for (int j = 0; j < 4; ++j){
			if (i == j){
				result.m[i][j] = 1.0f; // 対角成分は1
			} else{
				result.m[i][j] = 0.0f; // 対角以外の成分は0
			}
		}
	}
	return result;
}

Matrix4x4 Matrix4x4::Inverse(const Matrix4x4& mat){
	Matrix4x4 result;

	//行列式を求める
#pragma region 行列式
	float bottom =
		mat.m[0][0] * mat.m[1][1] * mat.m[2][2] * mat.m[3][3] +
		mat.m[0][0] * mat.m[1][2] * mat.m[2][3] * mat.m[3][1] +
		mat.m[0][0] * mat.m[1][3] * mat.m[2][1] * mat.m[3][2] -

		mat.m[0][0] * mat.m[1][3] * mat.m[2][2] * mat.m[3][1] -
		mat.m[0][0] * mat.m[1][2] * mat.m[2][1] * mat.m[3][3] -
		mat.m[0][0] * mat.m[1][1] * mat.m[2][3] * mat.m[3][2] -

		mat.m[0][1] * mat.m[1][0] * mat.m[2][2] * mat.m[3][3] -
		mat.m[0][2] * mat.m[1][0] * mat.m[2][3] * mat.m[3][1] -
		mat.m[0][3] * mat.m[1][0] * mat.m[2][1] * mat.m[3][2] +

		mat.m[0][3] * mat.m[1][0] * mat.m[2][2] * mat.m[3][1] +
		mat.m[0][2] * mat.m[1][0] * mat.m[2][1] * mat.m[3][3] +
		mat.m[0][1] * mat.m[1][0] * mat.m[2][3] * mat.m[3][2] +

		mat.m[0][1] * mat.m[1][2] * mat.m[2][0] * mat.m[3][3] +
		mat.m[0][2] * mat.m[1][3] * mat.m[2][0] * mat.m[3][1] +
		mat.m[0][3] * mat.m[1][1] * mat.m[2][0] * mat.m[3][2] -

		mat.m[0][3] * mat.m[1][2] * mat.m[2][0] * mat.m[3][1] -
		mat.m[0][2] * mat.m[1][1] * mat.m[2][0] * mat.m[3][3] -
		mat.m[0][1] * mat.m[1][3] * mat.m[2][0] * mat.m[3][2] -

		mat.m[0][1] * mat.m[1][2] * mat.m[2][3] * mat.m[3][0] -
		mat.m[0][2] * mat.m[1][3] * mat.m[2][1] * mat.m[3][0] -
		mat.m[0][3] * mat.m[1][1] * mat.m[2][2] * mat.m[3][0] +

		mat.m[0][3] * mat.m[1][2] * mat.m[2][1] * mat.m[3][0] +
		mat.m[0][2] * mat.m[1][1] * mat.m[2][3] * mat.m[3][0] +
		mat.m[0][1] * mat.m[1][3] * mat.m[2][2] * mat.m[3][0];
#pragma endregion

	float determinant = 1 / bottom;

	//逆行列を求める
#pragma region 1行目
	//======================================================
	result.m[0][0] =
		(mat.m[1][1] * mat.m[2][2] * mat.m[3][3] +
		 mat.m[1][2] * mat.m[2][3] * mat.m[3][1] +
		 mat.m[1][3] * mat.m[2][1] * mat.m[3][2] -
		 mat.m[1][3] * mat.m[2][2] * mat.m[3][1] -
		 mat.m[1][2] * mat.m[2][1] * mat.m[3][3] -
		 mat.m[1][1] * mat.m[2][3] * mat.m[3][2]) * determinant;

	result.m[0][1] =
		(-mat.m[0][1] * mat.m[2][2] * mat.m[3][3] -
		 mat.m[0][2] * mat.m[2][3] * mat.m[3][1] -
		 mat.m[0][3] * mat.m[2][1] * mat.m[3][2] +
		 mat.m[0][3] * mat.m[2][2] * mat.m[3][1] +
		 mat.m[0][2] * mat.m[2][1] * mat.m[3][3] +
		 mat.m[0][1] * mat.m[2][3] * mat.m[3][2]) * determinant;

	result.m[0][2] =
		(mat.m[0][1] * mat.m[1][2] * mat.m[3][3] +
		 mat.m[0][2] * mat.m[1][3] * mat.m[3][1] +
		 mat.m[0][3] * mat.m[1][1] * mat.m[3][2] -
		 mat.m[0][3] * mat.m[1][2] * mat.m[3][1] -
		 mat.m[0][2] * mat.m[1][1] * mat.m[3][3] -
		 mat.m[0][1] * mat.m[1][3] * mat.m[3][2]) * determinant;

	result.m[0][3] =
		(-mat.m[0][1] * mat.m[1][2] * mat.m[2][3] -
		 mat.m[0][2] * mat.m[1][3] * mat.m[2][1] -
		 mat.m[0][3] * mat.m[1][1] * mat.m[2][2] +
		 mat.m[0][3] * mat.m[1][2] * mat.m[2][1] +
		 mat.m[0][2] * mat.m[1][1] * mat.m[2][3] +
		 mat.m[0][1] * mat.m[1][3] * mat.m[2][2]) * determinant;
#pragma endregion

#pragma region 2行目
	//======================================================
	result.m[1][0] =
		(-mat.m[1][0] * mat.m[2][2] * mat.m[3][3] -
		 mat.m[1][2] * mat.m[2][3] * mat.m[3][0] -
		 mat.m[1][3] * mat.m[2][0] * mat.m[3][2] +
		 mat.m[1][3] * mat.m[2][2] * mat.m[3][0] +
		 mat.m[1][2] * mat.m[2][0] * mat.m[3][3] +
		 mat.m[1][0] * mat.m[2][3] * mat.m[3][2]) * determinant;

	result.m[1][1] =
		(mat.m[0][0] * mat.m[2][2] * mat.m[3][3] +
		 mat.m[0][2] * mat.m[2][3] * mat.m[3][0] +
		 mat.m[0][3] * mat.m[2][0] * mat.m[3][2] -
		 mat.m[0][3] * mat.m[2][2] * mat.m[3][0] -
		 mat.m[0][2] * mat.m[2][0] * mat.m[3][3] -
		 mat.m[0][0] * mat.m[2][3] * mat.m[3][2]) * determinant;

	result.m[1][2] =
		(-mat.m[0][0] * mat.m[1][2] * mat.m[3][3] -
		 mat.m[0][2] * mat.m[1][3] * mat.m[3][0] -
		 mat.m[0][3] * mat.m[1][0] * mat.m[3][2] +
		 mat.m[0][3] * mat.m[1][2] * mat.m[3][0] +
		 mat.m[0][2] * mat.m[1][0] * mat.m[3][3] +
		 mat.m[0][0] * mat.m[1][3] * mat.m[3][2]) * determinant;

	result.m[1][3] =
		(mat.m[0][0] * mat.m[1][2] * mat.m[2][3] +
		 mat.m[0][2] * mat.m[1][3] * mat.m[2][0] +
		 mat.m[0][3] * mat.m[1][0] * mat.m[2][2] -
		 mat.m[0][3] * mat.m[1][2] * mat.m[2][0] -
		 mat.m[0][2] * mat.m[1][0] * mat.m[2][3] -
		 mat.m[0][0] * mat.m[1][3] * mat.m[2][2]) * determinant;
#pragma endregion

#pragma region 3行目
	//======================================================
	result.m[2][0] =
		(mat.m[1][0] * mat.m[2][1] * mat.m[3][3] +
		 mat.m[1][1] * mat.m[2][3] * mat.m[3][0] +
		 mat.m[1][3] * mat.m[2][0] * mat.m[3][1] -
		 mat.m[1][3] * mat.m[2][1] * mat.m[3][0] -
		 mat.m[1][1] * mat.m[2][0] * mat.m[3][3] -
		 mat.m[1][0] * mat.m[2][3] * mat.m[3][1]) * determinant;

	result.m[2][1] =
		(-mat.m[0][0] * mat.m[2][1] * mat.m[3][3] -
		 mat.m[0][1] * mat.m[2][3] * mat.m[3][0] -
		 mat.m[0][3] * mat.m[2][0] * mat.m[3][1] +
		 mat.m[0][3] * mat.m[2][1] * mat.m[3][0] +
		 mat.m[0][1] * mat.m[2][0] * mat.m[3][3] +
		 mat.m[0][0] * mat.m[2][3] * mat.m[3][1]) * determinant;

	result.m[2][2] =
		(mat.m[0][0] * mat.m[1][1] * mat.m[3][3] +
		 mat.m[0][1] * mat.m[1][3] * mat.m[3][0] +
		 mat.m[0][3] * mat.m[1][0] * mat.m[3][1] -
		 mat.m[0][3] * mat.m[1][1] * mat.m[3][0] -
		 mat.m[0][1] * mat.m[1][0] * mat.m[3][3] -
		 mat.m[0][0] * mat.m[1][3] * mat.m[3][1]) * determinant;

	result.m[2][3] =
		(-mat.m[0][0] * mat.m[1][1] * mat.m[2][3] -
		 mat.m[0][1] * mat.m[1][3] * mat.m[2][0] -
		 mat.m[0][3] * mat.m[1][0] * mat.m[2][1] +
		 mat.m[0][3] * mat.m[1][1] * mat.m[2][0] +
		 mat.m[0][1] * mat.m[1][0] * mat.m[2][3] +
		 mat.m[0][0] * mat.m[1][3] * mat.m[2][1]) * determinant;
#pragma endregion

#pragma region 4行目
	//======================================================
	result.m[3][0] =
		(-mat.m[1][0] * mat.m[2][1] * mat.m[3][2] -
		 mat.m[1][1] * mat.m[2][2] * mat.m[3][0] -
		 mat.m[1][2] * mat.m[2][0] * mat.m[3][1] +
		 mat.m[1][2] * mat.m[2][1] * mat.m[3][0] +
		 mat.m[1][1] * mat.m[2][0] * mat.m[3][2] +
		 mat.m[1][0] * mat.m[2][2] * mat.m[3][1]) * determinant;

	result.m[3][1] =
		(mat.m[0][0] * mat.m[2][1] * mat.m[3][2] +
		 mat.m[0][1] * mat.m[2][2] * mat.m[3][0] +
		 mat.m[0][2] * mat.m[2][0] * mat.m[3][1] -
		 mat.m[0][2] * mat.m[2][1] * mat.m[3][0] -
		 mat.m[0][1] * mat.m[2][0] * mat.m[3][2] -
		 mat.m[0][0] * mat.m[2][2] * mat.m[3][1]) * determinant;

	result.m[3][2] =
		(-mat.m[0][0] * mat.m[1][1] * mat.m[3][2] -
		 mat.m[0][1] * mat.m[1][2] * mat.m[3][0] -
		 mat.m[0][2] * mat.m[1][0] * mat.m[3][1] +
		 mat.m[0][2] * mat.m[1][1] * mat.m[3][0] +
		 mat.m[0][1] * mat.m[1][0] * mat.m[3][2] +
		 mat.m[0][0] * mat.m[1][2] * mat.m[3][1]) * determinant;

	result.m[3][3] =
		(mat.m[0][0] * mat.m[1][1] * mat.m[2][2] +
		 mat.m[0][1] * mat.m[1][2] * mat.m[2][0] +
		 mat.m[0][2] * mat.m[1][0] * mat.m[2][1] -
		 mat.m[0][2] * mat.m[1][1] * mat.m[2][0] -
		 mat.m[0][1] * mat.m[1][0] * mat.m[2][2] -
		 mat.m[0][0] * mat.m[1][2] * mat.m[2][1]) * determinant;
#pragma endregion

	return result;
}

// 座標系変換
Vector3 Matrix4x4::Transform(const Vector3& vector, const Matrix4x4& matrix){
	Vector3 result = {0, 0, 0};

	// 同次座標系への変換
	// 変換行列を適用
	Vector4 homogeneousCoordinate(
		vector.x * matrix.m[0][0] + vector.y * matrix.m[1][0] + vector.z * matrix.m[2][0] + matrix.m[3][0],
		vector.x * matrix.m[0][1] + vector.y * matrix.m[1][1] + vector.z * matrix.m[2][1] + matrix.m[3][1],
		vector.x * matrix.m[0][2] + vector.y * matrix.m[1][2] + vector.z * matrix.m[2][2] + matrix.m[3][2],
		vector.x * matrix.m[0][3] + vector.y * matrix.m[1][3] + vector.z * matrix.m[2][3] + matrix.m[3][3]);

	// 同次座標系から3次元座標系に戻す
	float w = homogeneousCoordinate.w;
	assert(w != 0.0f); // wが0でないことを確認
	result.x = homogeneousCoordinate.x / w;
	result.y = homogeneousCoordinate.y / w;
	result.z = homogeneousCoordinate.z / w;

	return result;
}

Vector3 Matrix4x4::LookAtDirection(const Vector3& position, const Vector3& target){
	Vector3 forward = (target - position).Normalize();

	// ヨー (Y 軸の回転) とピッチ (X 軸の回転) を計算
	float yaw = std::atan2(forward.x, forward.z);
	float pitch = std::atan2(-forward.y, std::sqrt(forward.x * forward.x + forward.z * forward.z));

	// ロールはここでは 0 に設定（通常の LookAt では使わないため）
	float roll = 0.0f;

	return {pitch, yaw, roll};
}

Matrix4x4 Matrix4x4::MakeViewportMatrix(float l, float t, float w, float h, float minDepth, float maxDepth){
	Matrix4x4 result;
	result = {w / 2, 0, 0, 0, 0, -h / 2, 0, 0, 0, 0, maxDepth - minDepth, 0, l + (w / 2), t + (h / 2), minDepth, 1};
	return result;
}
