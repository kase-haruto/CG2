#include "Particle.h"
#include "DirectXCommon.h"
#include"myfunc/MyFunc.h"
#include "VertexData.h"
#include "TextureManager.h"
#include "GraphicsGroup.h"
#include "DirectionalLight.h"
#include "SrvLocator.h"
#include "objects/ModelManager.h"

#ifdef _DEBUG
#include "imgui.h"
#endif // _DEBUG

#include<string>
#include<numbers>


ParticleManager::ParticleManager(const uint32_t kInstanceNum) :kMaxInstance_(kInstanceNum){}

ParticleManager::~ParticleManager(){}

void ParticleManager::Initialize(ViewProjection* viewProjection){
	device_ = GraphicsGroup::GetInstance()->GetDevice();
	commandList_ = GraphicsGroup::GetInstance()->GetCommandList();
	rootSignature_ = GraphicsGroup::GetInstance()->GetRootSignature(StructuredObject);
	pipelineState_ = GraphicsGroup::GetInstance()->GetPipelineState(StructuredObject);
	viewProjection_ = viewProjection;

	backToFrontMatrix_ = MakeRotateYMatrix(std::numbers::pi_v<float>);

	modelData = ModelManager::LoadModel("plane");
	RGBa = {1.0f, 1.0f, 1.0f, 1.0f};



	handle = TextureManager::GetInstance()->LoadTexture("particle.png");
	if (!handle.ptr){
		handle = TextureManager::GetInstance()->LoadTexture("white1x1.png");
	}

	CreateBuffer();
	Map();
	CreateSRV();

	//エミッターの初期化
	emitter.count = 3;
	emitter.transform.translate = {0.0f,0.0f,0.0f};
	emitter.transform.rotate = {0.0f,0.0f,0.0f};
	emitter.transform.scale = {1.0f,1.0f,1.0f};
	emitter.frequency = 0.5f;
	emitter.frequencyTime = 0.0f;

	//フィールドの初期化
	accelerationField.acceleration = {15.0f,0.0f,0.0f};
	Vector3 min = {-1.0f,-1.0f,-1.0f};
	Vector3 max = {1.0f,1.0f,1.0f};
	accelerationField.area.Initialize(min,max);
	accelerationField.isUpdate = false;
}

Particle ParticleManager::MakeNewParticle(std::mt19937& randomEngine,const Vector3& translate){
	Particle newParticle;
	std::uniform_real_distribution<float>distribution(-1.0, 1.0f);
	std::uniform_real_distribution<float>distColor(0.0f, 1.0f);
	std::uniform_real_distribution<float>distTIme(1.0f, 3.0f);

	/*  transform,velocity,color,lifetime,currentTime の初期化  */
	Vector3 randomTranslate {distribution(randomEngine),distribution(randomEngine),distribution(randomEngine)};
	newParticle.transform.translate = {translate.x + randomTranslate.x,translate.y + randomTranslate.y,translate.z + randomTranslate.z};
	newParticle.transform.scale = {1.0f,1.0f,1.0f};
	newParticle.transform.rotate = {0.0f, 0.0f, 0.0f};
	newParticle.velocity = {distribution(randomEngine),distribution(randomEngine),distribution(randomEngine)};
	newParticle.color = {distColor(randomEngine),distColor(randomEngine),distColor(randomEngine),1.0f};
	newParticle.lifeTime = distTIme(randomEngine);
	newParticle.currentTime = 0;

	return newParticle;
}

void ParticleManager::Create(ViewProjection* viewProjection){
	// ランダム生成器の初期化
	std::random_device rd;
	std::mt19937 randomEngine(rd());
	std::uniform_real_distribution<float> distribution(-1.0f, 1.0f);

	// kMaxInstance_ 分のパーティクルをリストに追加
	for (uint32_t index = 0; index < kMaxInstance_; ++index){
		particle_.emplace_back(MakeNewParticle(randomEngine,emitter.transform.translate));
	}

	// 初期化
	Initialize(viewProjection);
}


void ParticleManager::CreateSRV(){
	auto [srvHandleCPU, srvHandleGPU] = SrvLocator::AllocateSrv();

	D3D12_SHADER_RESOURCE_VIEW_DESC instancingSrvDesc = {};
	instancingSrvDesc.Format = DXGI_FORMAT_UNKNOWN;
	instancingSrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	instancingSrvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	instancingSrvDesc.Buffer.FirstElement = 0;
	instancingSrvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
	instancingSrvDesc.Buffer.NumElements = kMaxInstance_;
	instancingSrvDesc.Buffer.StructureByteStride = sizeof(ParticleForGPU);

	device_->CreateShaderResourceView(instancingResource_.Get(), &instancingSrvDesc, srvHandleCPU);
	instancingSrvHandleGPU_ = srvHandleGPU;
}

void ParticleManager::Update(){
#ifdef _DEBUG
	ImGui::Begin("ParticleManager");
	std::random_device rd;
	std::mt19937 randomEngine(rd());
	if (ImGui::Button("Add particle")){
		particle_.splice(particle_.end(), Emit(emitter, randomEngine));
	}
	ImGui::Checkbox("isFieldUpdate", &accelerationField.isUpdate);
	ImGui::DragFloat3("EmitterTranslate", &emitter.transform.translate.x, 0.01f, -100.0f, 100.0f);
	ImGui::End();
#endif // _DEBUG

	materialData->color = RGBa;
	numInstance_ = 0;
	for (auto it = particle_.begin(); it != particle_.end(); ){

		if (numInstance_ < kMaxInstance_){

			if (it->lifeTime <= it->currentTime){ // 生存時間を過ぎた場合
				it = particle_.erase(it); // パーティクルを削除し、次のイテレータに進む
				continue;
			}

			Matrix4x4 billboardMatrix = Matrix4x4::Multiply(backToFrontMatrix_, viewProjection_->cameraMatrix);
			billboardMatrix.m[3][0] = 0.0f;
			billboardMatrix.m[3][1] = 0.0f;
			billboardMatrix.m[3][2] = 0.0f;

			Matrix4x4 scaleMatrix = MakeScaleMatrix(it->transform.scale);
			Matrix4x4 translateMatrix = MakeTranslateMatrix(it->transform.translate);
			Matrix4x4 worldMatrix = Matrix4x4::Multiply(Matrix4x4::Multiply(scaleMatrix, billboardMatrix), translateMatrix);
			Matrix4x4 worldViewProjectionMatrix = Matrix4x4::Multiply(worldMatrix, viewProjection_->GetViewProjection());

			instancingData[numInstance_].WVP = worldViewProjectionMatrix;
			instancingData[numInstance_].World = worldMatrix;
			instancingData[numInstance_].color = it->color;
			float alpha = 1.0f - (it->currentTime / it->lifeTime);
			instancingData[numInstance_].color.w = alpha;

			//フィールドの更新処理
			if (accelerationField.isUpdate){
				if (IsCollision(accelerationField.area, it->transform.translate)){
					it->velocity += accelerationField.acceleration * deltaTime;
				}
			}
			

			// 座標の更新
			it->transform.translate.x += it->velocity.x * deltaTime;
			it->transform.translate.y += it->velocity.y * deltaTime;
			it->transform.translate.z += it->velocity.z * deltaTime;
			it->currentTime += deltaTime; // 経過時間を増やす

			++numInstance_; // パーティクルのカウントを増やす
		}



		++it; // 次のパーティクルへ進む
	}

	//エミッターの更新処理
	emitter.frequencyTime += deltaTime;	//時刻を決める
	if (emitter.frequency<= emitter.frequencyTime){
		particle_.splice(particle_.end(), Emit(emitter, randomEngine));
		emitter.frequencyTime -= emitter.frequency;
	}

	
}

void ParticleManager::Draw(){
	// ルートシグネチャを設定
	commandList_->SetGraphicsRootSignature(rootSignature_.Get());

	// パイプラインステートを設定
	commandList_->SetPipelineState(pipelineState_.Get());

	// 頂点バッファを設定
	commandList_->IASetVertexBuffers(0, 1, &vertexBufferView);

	// プリミティブトポロジーを設定（ここでは三角形リストを指定）
	commandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// マテリアル用の定数バッファをルートパラメータ0にバインド
	commandList_->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());

	// インスタンシング用のSRV（Shader Resource View）をルートパラメータ1にバインド
	commandList_->SetGraphicsRootDescriptorTable(1, instancingSrvHandleGPU_);

	// テクスチャ用のSRVをルートパラメータ2にバインド
	commandList_->SetGraphicsRootDescriptorTable(2, handle);

	// インスタンシングを用いて、指定された頂点バッファの頂点数分だけ描画を行う
	commandList_->DrawInstanced(static_cast< UINT >(modelData->vertices.size()), numInstance_, 0, 0);
}

void ParticleManager::CreateBuffer(){
	CreateVertexBuffer();
	CreateMaterialBuffer();
	CreateMatrixBuffer();
}

void ParticleManager::CreateVertexBuffer(){
	vertexResource_ = CreateBufferResource(device_, sizeof(VertexData) * modelData->vertices.size());
	vertexBufferView.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	vertexBufferView.SizeInBytes = static_cast< UINT >(sizeof(VertexData) * modelData->vertices.size());
	vertexBufferView.StrideInBytes = sizeof(VertexData);
}

void ParticleManager::CreateMaterialBuffer(){
	materialResource_ = CreateBufferResource(device_, sizeof(Material));
}

void ParticleManager::CreateMatrixBuffer(){
	instancingResource_ = CreateBufferResource(device_, sizeof(ParticleForGPU) * kMaxInstance_);
}

void ParticleManager::Map(){
	VertexBufferMap();
	MaterialBufferMap();
	MatrixBufferMap();
}

void ParticleManager::VertexBufferMap(){
	VertexData* vertexData = nullptr;
	HRESULT hr = vertexResource_->Map(0, nullptr, reinterpret_cast< void** >(&vertexData));
	if (FAILED(hr)){
		throw std::runtime_error("Failed to map vertex buffer.");
	}
	std::memcpy(vertexData, modelData->vertices.data(), sizeof(VertexData) * modelData->vertices.size());
}

void ParticleManager::MaterialBufferMap(){
	HRESULT hr = materialResource_->Map(0, nullptr, reinterpret_cast< void** >(&materialData));
	if (FAILED(hr)){
		throw std::runtime_error("Failed to map material buffer.");
	}
	materialData->color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	materialData->enableLighting = Lambert;
	materialData->uvTransform = Matrix4x4::MakeIdentity();
}

void ParticleManager::MatrixBufferMap(){
	HRESULT hr = instancingResource_->Map(0, nullptr, reinterpret_cast< void** >(&instancingData));
	if (FAILED(hr)){
		throw std::runtime_error("Failed to map instance buffer.");
	}

	uint32_t index = 0;
	for (auto it = particle_.begin(); it != particle_.end(); ++it, ++index){
		instancingData[index].WVP = Matrix4x4::MakeIdentity();
		instancingData[index].World = Matrix4x4::MakeIdentity();
		instancingData[index].color = it->color;
	}
}

std::list<Particle> ParticleManager::Emit(const Emitter& emitter, std::mt19937& randomEngine){
	std::list<Particle> particles;
	for (uint32_t count = 0; count < emitter.count; ++count){
		particles.push_back(MakeNewParticle(randomEngine,emitter.transform.translate));
	}
	return particles;
}