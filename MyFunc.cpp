#include"MyFunc.h"
#include<cmath>
#include<fstream>
#include<sstream>
#include<cassert>

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

ID3D12Resource* CreateBufferResource(ID3D12Device* device, size_t sizeInBytes){
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
	ID3D12Resource* bufferResource = nullptr;
	HRESULT hr = device->CreateCommittedResource(&uploadHeapProperties, D3D12_HEAP_FLAG_NONE,
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
	ModelData modelData;
	std::vector<Vector4> positions;
	std::vector<Vector3> normals;
	std::vector<Vector2> texcoords;
	std::string line;

	// ファイルを開く
	std::ifstream file(directoryPath + "/" + filename);
	assert(file.is_open());

	// ファイルの読み込み
	while (std::getline(file, line)){
		std::string identifier;
		std::istringstream s(line);
		s >> identifier; // 先頭の識別子を読む

		if (identifier == "v"){ // 頂点位置
			Vector4 position;
			s >> position.x >> position.y >> position.z;
			position.w = 1.0f;
			positions.push_back(position);
		} else if (identifier == "vt"){ // 頂点テクスチャ
			Vector2 texcoord;
			s >> texcoord.x >> texcoord.y;
			texcoord.y = 1 - texcoord.y;
			texcoords.push_back(texcoord);
		} else if (identifier == "vn"){ // 頂点法線
			Vector3 normal;
			s >> normal.x >> normal.y >> normal.z;
			normals.push_back(normal);
		} else if (identifier == "f"){ // 面
			VertexData triangle[3];
			// 面は三角形限定 その他は未対応
			for (int32_t faceVertex = 0; faceVertex < 3; ++faceVertex){
				std::string vertexDefinition;
				s >> vertexDefinition;
				// 頂点要素へのindexは[位置・UV・法線]で格納されているので分解してindexを取得する
				std::istringstream v(vertexDefinition);
				std::string index;
				uint32_t positionIndex = 0, texcoordIndex = 0, normalIndex = 0;

				std::getline(v, index, '/');
				positionIndex = std::stoi(index);

				if (std::getline(v, index, '/')){
					if (!index.empty()){
						texcoordIndex = std::stoi(index);
					}
				}

				if (std::getline(v, index, '/')){
					normalIndex = std::stoi(index);
				}

				// 要素へのindexから、実際の要素の値を取得して、頂点を構築する
				Vector4 position = positions[positionIndex - 1];
				Vector2 texcoord = texcoordIndex > 0 ? texcoords[texcoordIndex - 1] : Vector2 {1.0f, 1.0f};
				Vector3 normal = normals[normalIndex - 1];

				// 右手座標系から左手座標系に変換（X軸を反転）
				position.x *= -1.0f;
				normal.x *= -1.0f;

				triangle[faceVertex] = {position, texcoord, normal};
			}
			modelData.vertices.push_back(triangle[2]);
			modelData.vertices.push_back(triangle[1]);
			modelData.vertices.push_back(triangle[0]);
		} else if (identifier == "mtllib"){
			// materialTemplateLibraryファイルの名前を取得する
			std::string materialFilename;
			s >> materialFilename;
			// 基本的にobjファイルと同一階層
			modelData.material = LoadMaterialTemplateFile(directoryPath, materialFilename);
		}
	}
	return modelData;
}


MaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename){
	MaterialData materialData;//構築するmaterialData
	std::string line;//ファイルから読んだ1行を格納するもの
	std::ifstream file(directoryPath + "/" + filename);
	assert(file.is_open());//開かなかったら止める

	while (std::getline(file, line)){
		std::string identifier; std::istringstream s(line);
		s >> identifier;

		//identifierに応じた処理
		if (identifier == "map_Kd"){
			std::string textureFilename;
			s >> textureFilename;
			//連結してファイルパスにする
			materialData.textureFilePath = directoryPath + "/" + textureFilename;
		}
	}
	return materialData;
}