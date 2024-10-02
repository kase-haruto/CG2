#include"MyFunc.h"
#include"ConvertString.h"
#include<cmath>
#include<fstream>
#include<sstream>
#include<cassert>
#include<assimp/Importer.hpp>
#include<assimp/scene.h>
#include<assimp/postprocess.h>

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
    Assimp::Importer importer;
    // ファイルを開く
    std::ifstream file(directoryPath + "/" + filename + "/" + filename + ".obj");
 //   const aiScene* scene = importer.ReadFile(filename.c_str(), aiProcess_FlipWindingOrder | aiProcess_FlipUVs);
    assert(file.is_open());// 失敗したらアサート
  //  assert(scene->HasMeshes());
    // 開けたら必要な変数を用意
    ModelData modelData;
    std::vector<Vector4>positions;
    std::vector<Vector3>normals;
    std::vector<Vector2>texcoords;
    std::string line;
    int oCount = 0;

    while (std::getline(file, line)){

        // まずobjファイルの行の先頭の識別子を読む
        std::string identifer;
        std::istringstream s(line);
        s >> identifer;

        // 識別子に応じた処理
        if (identifer == "o"){

            oCount++;

        } else if (identifer == "v"){// 頂点位置-----------------------------

            Vector4 position;
            // x,y,zの順に格納
            s >> position.x >> position.y >> position.z;
            // zは1.0固定
            position.w = 1.0f;
            // 座標配列末尾にを要素を追加
            positions.push_back(position);


        } else if (identifer == "vt"){// 頂点テクスチャ座標------------

            Vector2 texcoord;
            // x,y,zの順に格納
            s >> texcoord.x >> texcoord.y;
            // テクスチャ座標配列末尾にを要素を追加
            texcoords.push_back(texcoord);

        } else if (identifer == "vn"){// 頂点法線--------------------

            Vector3 normal;
            // x,y,zの順に格納
            s >> normal.x >> normal.y >> normal.z;
            // 座標配列末尾にを要素を追加
            normals.push_back(normal);

        } else if (identifer == "f"){// 面

            VertexData vertices[3];

            for (int32_t faceVertex = 0; faceVertex < 3; faceVertex++){

                std::string vertexDefinition;
                s >> vertexDefinition;
                std::istringstream v(vertexDefinition);
                uint32_t elementIndices[3];

                for (int32_t element = 0; element < 3; element++){
                    std::string index;
                    std::getline(v, index, '/');

                    if (index == ""){
                        elementIndices[element] = 0;
                    } else{
                        elementIndices[element] = std::stoi(index);

                    }
                }

                Vector4 position;
                Vector2 texcoord;
                Vector3 normal;

                // テクスチャありと無しに対応
                if (elementIndices[1] == 0){
                    texcoord = {0.0f,0.0f};
                } else{
                    texcoord = texcoords[elementIndices[1] - 1];
                }

                position = positions[elementIndices[0] - 1];
                normal = normals[elementIndices[2] - 1];

                // 左手座標系に変換
                position.x *= -1.0f;
                normal.x *= -1.0f;
                texcoord.y = 1.0f - texcoord.y;

                vertices[faceVertex] = {position,texcoord,normal};
            }

            // 反対周りに格納
            modelData.vertices.push_back(vertices[2]);
            modelData.vertices.push_back(vertices[1]);
            modelData.vertices.push_back(vertices[0]);

        } else if (identifer == "mtllib"){// mtlファイルの場所

            std::string materialFilename;
            // ファイル名を格納
            s >> materialFilename;

            // mtlファイルを読み込む
            modelData.material = LoadMaterialTemplateFile(directoryPath + "/" + filename, materialFilename);
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