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

//ModelData LoadObjFile(const std::string& directoryPath, const std::string& filename){
//    Assimp::Importer importer;
//
//    // ファイルパスを作成
//    std::string filePath = directoryPath + "/" + filename + "/" + filename + ".obj";
//
//    // ファイルを開く
//    std::ifstream file(filePath);
//    assert(file.is_open()); // ファイルが開けなければアサート
//
//    // モデルデータの初期化
//    ModelData modelData;
//    std::vector<Vector4> positions;
//    std::vector<Vector3> normals;
//    std::vector<Vector2> texcoords;
//
//    std::string line;
//    uint32_t vertexIndex = 0;
//    while (std::getline(file, line)){
//        std::string identifier;
//        std::istringstream s(line);
//        s >> identifier;
//
//        if (identifier == "v"){
//            Vector4 position;
//            s >> position.x >> position.y >> position.z;
//            position.w = 1.0f;
//            positions.push_back(position);
//
//        } else if (identifier == "vt"){
//            Vector2 texcoord;
//            s >> texcoord.x >> texcoord.y;
//            texcoords.push_back(texcoord);
//
//        } else if (identifier == "vn"){
//            Vector3 normal;
//            s >> normal.x >> normal.y >> normal.z;
//            normals.push_back(normal);
//
//        } else if (identifier == "f"){
//            uint32_t faceIndices[3];
//            for (int32_t faceVertex = 0; faceVertex < 3; faceVertex++){
//                std::string vertexDefinition;
//                s >> vertexDefinition;
//
//                std::istringstream v(vertexDefinition);
//                uint32_t elementIndices[3] = {0, 0, 0};
//
//                for (int32_t element = 0; element < 3; element++){
//                    std::string index;
//                    std::getline(v, index, '/');
//                    if (!index.empty()){
//                        elementIndices[element] = std::stoi(index);
//                    }
//                }
//
//                Vector4 position = positions[elementIndices[0] - 1];
//                Vector2 texcoord = (elementIndices[1] != 0) ? texcoords[elementIndices[1] - 1] : Vector2 {0.0f, 0.0f};
//                Vector3 normal = normals[elementIndices[2] - 1];
//
//                // 左手座標系に変換
//                position.x *= -1.0f;
//                normal.x *= -1.0f;
//                texcoord.y = 1.0f - texcoord.y;
//
//                // 頂点データを格納
//                VertexData vertex {position, texcoord, normal};
//                modelData.vertices.push_back(vertex);
//                faceIndices[faceVertex] = vertexIndex++;
//            }
//
//            // 面を反時計回りで格納
//            modelData.indices.push_back(faceIndices[2]);
//            modelData.indices.push_back(faceIndices[1]);
//            modelData.indices.push_back(faceIndices[0]);
//        } else if (identifier == "mtllib"){
//            std::string materialFilename;
//            s >> materialFilename;
//
//            // mtlファイルを読み込む
//            modelData.material = LoadMaterialTemplateFile(directoryPath + "/" + filename, materialFilename);
//        }
//    }
//
//    return modelData;
//}

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