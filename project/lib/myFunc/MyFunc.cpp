#include"MyFunc.h"
#include"ConvertString.h"

#include "Engine/objects/Model/Model.h"

#include<cmath>
#include<fstream>
#include<sstream>
#include<cassert>
#include<assimp/Importer.hpp>
#include<assimp/scene.h>
#include<assimp/postprocess.h>
#include <numbers>

#include "Engine/graphics/camera/CameraManager.h"

//平行移動行列
Matrix4x4 MakeTranslateMatrix(const Vector3& translate){
	Matrix4x4 result = {
		1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		translate.x,translate.y,translate.z,1
	};
	return result;
}

//拡大縮小行列
Matrix4x4 MakeScaleMatrix(const Vector3& scale){
	Matrix4x4 result = {
		scale.x,0,0,0,
		0,scale.y,0,0,
		0,0,scale.z,0,
		0,0,0,1
	};
	return result;
}

//回転行列
Matrix4x4 MakeRotateXMatrix(float theta){
	Matrix4x4 result = {
		1,0,0,0,
		0,std::cos(theta),std::sin(theta),0,
		0,-std::sin(theta),std::cos(theta),0,
		0,0,0,1
	};

	return result;
}

Matrix4x4 MakeRotateYMatrix(float theta){
	Matrix4x4 result = {
		std::cos(theta),0,-std::sin(theta),0,
		0,1,0,0,
		std::sin(theta),0,std::cos(theta),0,
		0,0,0,1
	};
	return result;
}

Matrix4x4 MakeRotateZMatrix(float theta){
	Matrix4x4 result = {
		std::cos(theta),std::sin(theta),0,0,
		-std::sin(theta),std::cos(theta),0,0,
		0,0,1,0,
		0,0,0,1
	};
	return result;
}

Matrix4x4 EulerToMatrix(const Vector3& euler){
	Matrix4x4 rotateXMatrix = MakeRotateXMatrix(euler.x);
	Matrix4x4 rotateYMatrix = MakeRotateYMatrix(euler.y);
	Matrix4x4 rotateZMatrix = MakeRotateZMatrix(euler.z);
	return Matrix4x4::Multiply(Matrix4x4::Multiply(rotateXMatrix, rotateYMatrix), rotateZMatrix);
}

Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate){
	Matrix4x4 affineMatrix;
	Matrix4x4 translateMatrix = MakeTranslateMatrix(translate);
	Matrix4x4 scaleMatrix = MakeScaleMatrix(scale);

	Matrix4x4 rotateXMatrix = MakeRotateXMatrix(rotate.x);
	Matrix4x4 rotateYMatrix = MakeRotateYMatrix(rotate.y);
	Matrix4x4 rotateZMatrix = MakeRotateZMatrix(rotate.z);
	Matrix4x4 rotateMatrix = Matrix4x4::Multiply(Matrix4x4::Multiply(rotateXMatrix, rotateYMatrix), rotateZMatrix);

	affineMatrix = Matrix4x4::Multiply(Matrix4x4::Multiply(scaleMatrix, rotateMatrix), translateMatrix);

	return affineMatrix;
}

Matrix4x4 MakeOrthographicMatrix(float l, float t, float r, float b, float nearClip, float farClip){
	Matrix4x4 result;
	result = {
		2 / (r - l), 0, 0, 0,
		0, 2 / (t - b), 0, 0,
		0, 0, 1 / (farClip - nearClip), 0,
		(l + r) / (l - r), (t + b) / (b - t), nearClip / (nearClip - farClip), 1
	};
	return result;
}

Microsoft::WRL::ComPtr<ID3D12Resource>CreateBufferResource(Microsoft::WRL::ComPtr<ID3D12Device> device, size_t sizeInBytes){
	// 頂点リソース用のヒープの設定
	D3D12_HEAP_PROPERTIES uploadHeapProperties {};
	uploadHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;

	// 頂点リソースの設定
	D3D12_RESOURCE_DESC bufferResourceDesc {};
	// バッファリソース。テクスチャの場合はまた別の設定をする
	bufferResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	bufferResourceDesc.Width = sizeInBytes; // 指定されたサイズに設定
	// バッファの場合はこれらは1にする決まり
	bufferResourceDesc.Height = 1;
	bufferResourceDesc.DepthOrArraySize = 1;
	bufferResourceDesc.MipLevels = 1;
	bufferResourceDesc.SampleDesc.Count = 1;
	// バッファの場合はこれにする決まり
	bufferResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	// 実際にリソースを作る
	Microsoft::WRL::ComPtr<ID3D12Resource> bufferResource = nullptr;
	device->CreateCommittedResource(&uploadHeapProperties, D3D12_HEAP_FLAG_NONE,
									&bufferResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&bufferResource));

	return bufferResource;
}

D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(ID3D12DescriptorHeap* descriptorHeap, uint32_t descriptorSize, uint32_t index){
	D3D12_CPU_DESCRIPTOR_HANDLE handleCPU = descriptorHeap->GetCPUDescriptorHandleForHeapStart();
	handleCPU.ptr += (descriptorSize * index);
	return handleCPU;
}

D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(ID3D12DescriptorHeap* descriptorHeap, uint32_t descriptorSize, uint32_t index){
	D3D12_GPU_DESCRIPTOR_HANDLE handleGPU = descriptorHeap->GetGPUDescriptorHandleForHeapStart();
	handleGPU.ptr += (descriptorSize * index);
	return handleGPU;
}

ModelData LoadObjFile(const std::string& directoryPath, const std::string& filename){
	Assimp::Importer importer;

	// ファイルパスを作成
	std::string filePath = directoryPath + "/" + filename + "/" + filename + ".obj";

	// Assimpによるシーンの読み込み
	const aiScene* scene = importer.ReadFile(filePath, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
	assert(scene && scene->HasMeshes()); // 読み込みエラーやメッシュの有無を確認

	ModelData modelData;
	const aiMesh* mesh = scene->mMeshes[0]; // 最初のメッシュを取得

	// 頂点データの読み込み
	for (unsigned int i = 0; i < mesh->mNumVertices; i++){
		VertexData vertex;

		// 位置データの取得
		vertex.position.x = mesh->mVertices[i].x;
		vertex.position.y = mesh->mVertices[i].y;
		vertex.position.z = mesh->mVertices[i].z;
		vertex.position.w = 1.0f;

		// 法線データの取得
		if (mesh->HasNormals()){
			vertex.normal.x = mesh->mNormals[i].x;
			vertex.normal.y = mesh->mNormals[i].y;
			vertex.normal.z = mesh->mNormals[i].z;
		}

		// テクスチャ座標の取得
		if (mesh->HasTextureCoords(0)){
			vertex.texcoord.x = mesh->mTextureCoords[0][i].x;
			vertex.texcoord.y = mesh->mTextureCoords[0][i].y;
		} else{
			vertex.texcoord = {0.0f, 0.0f};
		}

		modelData.vertices.push_back(vertex);
	}

	// インデックスデータの読み込み
	for (unsigned int i = 0; i < mesh->mNumFaces; i++){
		const aiFace& face = mesh->mFaces[i];
		assert(face.mNumIndices == 3); // 三角形のみを想定

		modelData.indices.push_back(face.mIndices[0]);
		modelData.indices.push_back(face.mIndices[1]);
		modelData.indices.push_back(face.mIndices[2]);
	}

	// マテリアルの読み込み
	if (scene->HasMaterials()){
		const aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		aiString texturePath;
		if (material->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath) == AI_SUCCESS){
			modelData.material.textureFilePath = texturePath.C_Str();
		} else{
			modelData.material.textureFilePath = "white1x1.png";
		}
	}

	return modelData;
}


MaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename){

	// ファイルを開く
	std::ifstream file(directoryPath + "/" + filename);
	assert(file.is_open());// 失敗したらアサート

	MaterialData materialData;
	std::string line;

	while (std::getline(file, line)){

		// まずobjファイルの行の先頭の識別子を読む
		std::string identifer;
		std::istringstream s(line);
		s >> identifer;

		if (identifer == "map_Kd"){// ファイル名

			std::string textureFilename;
			Vector3 scale = {1.0f,1.0f,1.0f};
			Vector3 offset = {0.0f,0.0f,0.0f};
			Vector3 translate = {0.0f,0.0f,0.0f};

			// ファイル名を格納
			while (s >> textureFilename){
				if (textureFilename[0] == '-'){
					std::string option = textureFilename.substr(1);
					if (option == "s"){
						s >> scale.x >> scale.y >> scale.z;
					} else if (option == "o"){
						s >> offset.x >> offset.y >> offset.z;
					} else if (option == "t"){
						s >> translate.x >> translate.y >> translate.z;
					}
				} else{
					materialData.textureFilePath = textureFilename;
				}
			}

			materialData.uv_scale = scale;
			materialData.uv_offset = offset;
			materialData.uv_translate = translate;
		}
	}

	// テクスチャなしのモデルの場合
	if (materialData.textureFilePath == ""){
		materialData.textureFilePath = "white1x1.png";
	}

	return materialData;
}


DirectX::ScratchImage LoadTextureImage(const std::string& filePath){

	DirectX::ScratchImage image {};
	DirectX::ScratchImage mipImages {};

	std::wstring filePathW = ConvertString(filePath);// wstring型に変換
	// ファイルを読み込む
	HRESULT hr = DirectX::LoadFromWICFile(
		filePathW.c_str(),
		DirectX::WIC_FLAGS_FORCE_SRGB,
		nullptr,
		image
	);
	assert(SUCCEEDED(hr));

	// ミップマップの作成
	if (image.GetMetadata().width > 1 && image.GetMetadata().height > 1){
		hr = DirectX::GenerateMipMaps(
			image.GetImages(),
			image.GetImageCount(),
			image.GetMetadata(),
			DirectX::TEX_FILTER_SRGB,
			0,
			mipImages
		);
	} else{// サイズが1x1のときはここ
		return image;
	}

	assert(SUCCEEDED(hr));

	return mipImages;
}



bool IsCollision(const AABB& aabb, const Vector3& point){
	// pointがaabbのminとmaxの範囲内にあるかチェック
	return (point.x >= aabb.min.x && point.x <= aabb.max.x) &&
		(point.y >= aabb.min.y && point.y <= aabb.max.y) &&
		(point.z >= aabb.min.z && point.z <= aabb.max.z);
}

Vector3 TransformNormal(const Vector3& v, const Matrix4x4& m){
	Vector3 result {
		v.x * m.m[0][0] + v.y * m.m[1][0] + v.z * m.m[2][0],
		v.x * m.m[0][1] + v.y * m.m[1][1] + v.z * m.m[2][1],
		v.x * m.m[0][2] + v.y * m.m[1][2] + v.z * m.m[2][2]};
	return result;
}

float Lerp(float v1, float v2, float t){
	return v1 + (v2 - v1) * t;
}

float LerpShortAngle(float a, float b, float t){
	const float TWO_PI = 2.0f * ( float ) std::numbers::pi; // 2π (6.283185307179586)
	const float PI = ( float ) std::numbers::pi;            // π (3.141592653589793)

	// 角度差分を求める
	float diff = b - a;

	// 角度を[-π, π]に補正する
	diff = fmod(diff, TWO_PI);
	if (diff > PI){
		diff -= TWO_PI;
	} else if (diff < -PI){
		diff += TWO_PI;
	}

	// Lerpを使用して補間
	return Lerp(a, a + diff, t);

}


Vector3 ExtractEulerAnglesFromMatrix(const Matrix4x4& worldMatrix){
	// 仮定: 回転順序は YXZ など
	Vector3 euler;
	// row-major 前提での計算例（回転順序に応じて変更必要）
	euler.y = std::atan2(worldMatrix.m[0][2], worldMatrix.m[2][2]);
	float cosY = std::cos(euler.y);
	euler.x = std::atan2(-worldMatrix.m[1][2], worldMatrix.m[2][2] / cosY);
	euler.z = std::atan2(worldMatrix.m[0][1], worldMatrix.m[0][0]);
	return euler;
}

Vector2 WorldToScreen(const Vector3& worldPos){
	// ビューポート行列を作成
	Matrix4x4 matViewport = Matrix4x4::MakeViewportMatrix(0, 0, 1280.0f, 720.0f, 0, 1);

	// ビュー・プロジェクションの合成行列を計算
	Matrix4x4 matVPV = Matrix4x4::Multiply(CameraManager::GetViewProjectionMatrix(), matViewport);

	// ワールド空間の座標をビュー・プロジェクション行列で変換（クリップ座標）
	Vector3 screenPos = Vector3::Transform(worldPos, matVPV);
	// スクリーン座標を返す
	return Vector2(screenPos.x, screenPos.y);
}

Vector4 MultiplyMatrixVector(const Matrix4x4& mat, const Vector4& vec){
	return Vector4(
		mat.m[0][0] * vec.x + mat.m[1][0] * vec.y + mat.m[2][0] * vec.z + mat.m[3][0] * vec.w,
		mat.m[0][1] * vec.x + mat.m[1][1] * vec.y + mat.m[2][1] * vec.z + mat.m[3][1] * vec.w,
		mat.m[0][2] * vec.x + mat.m[1][2] * vec.y + mat.m[2][2] * vec.z + mat.m[3][2] * vec.w,
		mat.m[0][3] * vec.x + mat.m[1][3] * vec.y + mat.m[2][3] * vec.z + mat.m[3][3] * vec.w
	);
}

bool WorldToScreen(const Vector3& worldPos, Vector2& outScreenPos){
	// ビューポート行列を作成
	Matrix4x4 matViewport = Matrix4x4::MakeViewportMatrix(0, 0, 1280.0f, 720.0f, 0, 1);

	// ビュー・プロジェクションの合成行列を計算
	Matrix4x4 matVP = CameraManager::GetViewProjectionMatrix();

	// ワールド空間の座標をビュー・プロジェクション行列で変換（クリップ座標）
	Vector4 clipPos = MultiplyMatrixVector(matVP, Vector4(worldPos.x,worldPos.y,worldPos.z, 1));

	// 正規化デバイス座標（NDC）に変換
	if (clipPos.w != 0.0f){
		clipPos.x /= clipPos.w;
		clipPos.y /= clipPos.w;
		clipPos.z /= clipPos.w;
	}

	// 視錐台（クリップ座標）の範囲外にある場合は false を返す
	if (clipPos.x < -1.0f || clipPos.x > 1.0f ||
		clipPos.y < -1.0f || clipPos.y > 1.0f ||
		clipPos.z < 0.0f || clipPos.z > 1.0f){
		return false; // 見えていない
	}

	// ビューポート行列で変換し、スクリーン座標を計算
	Vector4 screenPos = MultiplyMatrixVector(matViewport, clipPos);
	outScreenPos = Vector2(screenPos.x, screenPos.y);

	return true; // 見えている
}

void DecomposeMatrix(const Matrix4x4& mat, Vector3& outScale, Vector3& outRotate, Vector3& outTrans){
		// ① 平行移動成分の抽出
		// ※ row-major の場合、4行目（インデックス 3）の 0～2列目に Translation が入っていると仮定
		outTrans.x = mat.m[3][0];
		outTrans.y = mat.m[3][1];
		outTrans.z = mat.m[3][2];

		// ② スケール成分の抽出
		// 各行の上位３成分の長さが各軸方向のスケール（シアーがない前提）
		outScale.x = std::sqrt(mat.m[0][0] * mat.m[0][0] +
							   mat.m[0][1] * mat.m[0][1] +
							   mat.m[0][2] * mat.m[0][2]);

		outScale.y = std::sqrt(mat.m[1][0] * mat.m[1][0] +
							   mat.m[1][1] * mat.m[1][1] +
							   mat.m[1][2] * mat.m[1][2]);

		outScale.z = std::sqrt(mat.m[2][0] * mat.m[2][0] +
							   mat.m[2][1] * mat.m[2][1] +
							   mat.m[2][2] * mat.m[2][2]);

		// ③ 回転成分の抽出
		// 上位3×3 部分からスケール成分を除く（各行を正規化）
		// ※ここでは rXY は「行 X, 列 Y」の要素
		float r00 = mat.m[0][0] / outScale.x;
		/*float r01 = mat.m[0][1] / outScale.x;
		float r02 = mat.m[0][2] / outScale.x;*/

		float r10 = mat.m[1][0] / outScale.y;
		float r11 = mat.m[1][1] / outScale.y;
		float r12 = mat.m[1][2] / outScale.y;

		float r20 = mat.m[2][0] / outScale.z;
		float r21 = mat.m[2][1] / outScale.z;
		float r22 = mat.m[2][2] / outScale.z;

		// オイラー角抽出（回転順序：X→Y→Z、つまり outRotate.x = pitch, outRotate.y = yaw, outRotate.z = roll）
		// ※以下は一般的な Tait-Bryan 角の抽出例です。※
		// まず sy = sqrt(r00² + r10²) を求め、特異点（ジンバルロック）をチェックします。
		float sy = std::sqrt(r00 * r00 + r10 * r10);
		const float EPSILON = 1e-6f;
		bool singular = sy < EPSILON;

		if (!singular){
			// 通常ケース
			outRotate.x = std::atan2(r21, r22);   // ピッチ（X軸回り）
			outRotate.y = std::atan2(-r20, sy);     // ヨー（Y軸回り）
			outRotate.z = std::atan2(r10, r00);     // ロール（Z軸回り）
		} else{
			// 特異点（ジンバルロック）の場合
			outRotate.x = std::atan2(-r12, r11);
			outRotate.y = std::atan2(-r20, sy);
			outRotate.z = 0.0f;
		}
}


/////////////////////////////////////////////////////////////////////////////////////////////
//							Animation
/////////////////////////////////////////////////////////////////////////////////////////////
Animation LoadAnimationFile(const std::string& directoryPath, const std::string& filename){
	Animation animation;// アニメーションデータ
	Assimp::Importer importer;
	std::string filePath = directoryPath + "/"
		+ filename.substr(0, filename.find_last_of('.')) + "/"
		+ filename;

	const aiScene* scene = importer.ReadFile(filePath.c_str(), 0);
	assert(scene->mNumAnimations);// アニメーションがない場合はアサート
	aiAnimation* animationAssimp = scene->mAnimations[0];// 最初のアニメーションを取得
	animation.duration = float(animationAssimp->mDuration / animationAssimp->mTicksPerSecond);// アニメーションの長さを取得

	// assimpでは個々のanimationをchannelとして読んでいるからchannelの数だけループ
	for (uint32_t channelIndex = 0; channelIndex < animationAssimp->mNumChannels; ++channelIndex){
		aiNodeAnim* nodeAnimationAssimp = animationAssimp->mChannels[channelIndex];// channelを取得
		NodeAnimation& nodeAnimation = animation.nodeAnimations[nodeAnimationAssimp->mNodeName.C_Str()];// ノードアニメーションを取得
		
		//translate
		for (uint32_t keyIndex = 0; keyIndex < nodeAnimationAssimp->mNumPositionKeys; ++keyIndex){
			aiVectorKey& keyAssimp = nodeAnimationAssimp->mPositionKeys[keyIndex];// キーフレームを取得
			KeyframeVector3 keyframe;

			keyframe.time = float(keyAssimp.mTime / animationAssimp->mTicksPerSecond);// キーフレームの時間を取得
			keyframe.value = {-keyAssimp.mValue.x, keyAssimp.mValue.y, keyAssimp.mValue.z};// キーフレームの値を取得 //<右手->左手座標系に変換>
			nodeAnimation.translate.keyframes.push_back(keyframe);// ノードアニメーションに追加

		}

		//rotate
		for (uint32_t keyIndex = 0; keyIndex < nodeAnimationAssimp->mNumRotationKeys; ++keyIndex){
			aiQuatKey& keyAssimp = nodeAnimationAssimp->mRotationKeys[keyIndex];
			KeyframeQuaternion keyframe;

			keyframe.time = float(keyAssimp.mTime / animationAssimp->mTicksPerSecond);
			// 右手->左手 (yとzの符号を反転)
			keyframe.value = {keyAssimp.mValue.x, -keyAssimp.mValue.y, -keyAssimp.mValue.z, keyAssimp.mValue.w};
			nodeAnimation.rotate.keyframes.push_back(keyframe);
		}

		//scale
		for (uint32_t keyIndex = 0; keyIndex < nodeAnimationAssimp->mNumScalingKeys; ++keyIndex){
			aiVectorKey& keyAssimp = nodeAnimationAssimp->mScalingKeys[keyIndex];
			KeyframeVector3 keyframe;
		
			keyframe.time = float(keyAssimp.mTime / animationAssimp->mTicksPerSecond);
			keyframe.value = {keyAssimp.mValue.x, keyAssimp.mValue.y, keyAssimp.mValue.z};
			nodeAnimation.scale.keyframes.push_back(keyframe);
		}

	}
	return animation;
}
