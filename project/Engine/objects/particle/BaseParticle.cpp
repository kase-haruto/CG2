#include "BaseParticle.h"

//* engine
#include "Engine/graphics/GraphicsGroup.h"
#include "Engine/physics/DirectionalLight.h"
#include "Engine/graphics/SrvLocator.h"
#include "Engine/objects/ModelManager.h"
#include "Engine/objects/TextureManager.h"
#include "Engine/graphics/camera/CameraManager.h"

//* lib //
#include "lib/myFunc/MyFunc.h"
#include "lib/myFunc/Random.h"

BaseParticle::BaseParticle(){

	particles_.clear();

}

void BaseParticle::Initialize(const std::string& modelName, const std::string& texturePath,const uint32_t count){

	Emit(count);


	/* modelとmaterial生成 =======================*/
	modelData_ = ModelManager::LoadModel(modelName);
	textureHandle = TextureManager::GetInstance()->LoadTexture(texturePath);

	backToFrontMatrix_ = MakeRotateYMatrix(std::numbers::pi_v<float>);

	/* リソースの生成 =======================*/
	CreateBuffer();
	Map();
	CreateSRV();

}

void BaseParticle::Update(){
	instanceNum_ = 0;
	for (auto it = particles_.begin(); it != particles_.end(); ){

		if (instanceNum_ < kMaxInstanceNum_){

			if (it->lifeTime <= it->currentTime){	// 生存時間を過ぎた場合
				it = particles_.erase(it);			// パーティクルを削除し、次のイテレータに進む
				continue;
			}


			Matrix4x4 billboardMatrix = Matrix4x4::Multiply(backToFrontMatrix_, CameraManager::GetCamera3d()->GetWorldMat());
			billboardMatrix.m[3][0] = 0.0f;
			billboardMatrix.m[3][1] = 0.0f;
			billboardMatrix.m[3][2] = 0.0f;

			Matrix4x4 scaleMatrix = MakeScaleMatrix(it->transform.scale);
			Matrix4x4 translateMatrix = MakeTranslateMatrix(it->transform.translate);
			Matrix4x4 worldMatrix = Matrix4x4::Multiply(Matrix4x4::Multiply(scaleMatrix, billboardMatrix), translateMatrix);
			Matrix4x4 worldViewProjectionMatrix = Matrix4x4::Multiply(worldMatrix, CameraManager::GetCamera3d()->GetViewProjectionMatrix());

			instancingData[instanceNum_].WVP = worldViewProjectionMatrix;
			instancingData[instanceNum_].World = worldMatrix;
			instancingData[instanceNum_].color = it->color;
			float alpha = 1.0f - (it->currentTime / it->lifeTime);
			instancingData[instanceNum_].color.w = alpha;



			// 生存時間の更新
			it->currentTime += deltaTime;

			//　座標の更新
			it->transform.translate += it->velocity * deltaTime;

			++instanceNum_;

		}

		++it;

	}

}

void BaseParticle::Draw(){

	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>commandList = GraphicsGroup::GetInstance()->GetCommandList();

	// 頂点バッファを設定
	commandList->IASetVertexBuffers(0, 1, &vertexBufferView);

	// マテリアル用定数バッファ
	commandList->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());

	// インスタンシング用srv
	commandList->SetGraphicsRootDescriptorTable(1, instancingSrvHandleGPU_);

	// texture用srv
	commandList->SetGraphicsRootDescriptorTable(2, textureHandle);

	// インスタンシングを用いて、指定された頂点バッファの頂点数分だけ描画を行う
	commandList->DrawInstanced(static_cast< UINT >(modelData_->vertices.size()), instanceNum_, 0, 0);

}

void BaseParticle::Emit(uint32_t count){
	for (uint32_t i = 0; i < count; ++i){
		if (particles_.size() >= static_cast< size_t >(kMaxInstanceNum_)){
			break; // 最大数に達した場合は生成を中断
		}

		ParticleData::Parameters particle;
		particle.SetColorRandom();
		particle.SetVelocityRandom(-1.0f, 1.0f);
		particle.lifeTime = Random::Generate(1.0f, 5.0f); // ライフタイムをランダムに設定
		particle.transform.translate = emitter_.transform.translate; // エミッタの位置に生成
		particles_.push_back(particle);
	}

	instanceNum_ = static_cast< int32_t >(particles_.size());
}

void BaseParticle::CreateBuffer(){

	Microsoft::WRL::ComPtr<ID3D12Device> device = GraphicsGroup::GetInstance()->GetDevice();

	vertexBuffer_ = CreateBufferResource(device, sizeof(VertexData) * modelData_->vertices.size());
	vertexBufferView.BufferLocation = vertexBuffer_->GetGPUVirtualAddress();
	vertexBufferView.SizeInBytes = static_cast< UINT >(sizeof(VertexData) * modelData_->vertices.size());
	vertexBufferView.StrideInBytes = sizeof(VertexData);

	//マテリアル用
	materialResource_ = CreateBufferResource(device.Get(), sizeof(Material));

	//インスタンシング用
	instancingResource_ = CreateBufferResource(device.Get(), sizeof(ParticleData::ParticleForGPU) * kMaxInstanceNum_);

}

void BaseParticle::Map(){

	/* vertex =======================*/
	VertexData* vertexData = nullptr;
	HRESULT hr = vertexBuffer_->Map(0, nullptr, reinterpret_cast< void** >(&vertexData));
	if (FAILED(hr)){
		throw std::runtime_error("Failed to map vertex buffer.");
	}
	std::memcpy(vertexData, modelData_->vertices.data(), sizeof(VertexData) * modelData_->vertices.size());


	/* material =======================*/
	HRESULT materialHr = materialResource_->Map(0, nullptr, reinterpret_cast< void** >(&materialData));
	if (FAILED(materialHr)){
		throw std::runtime_error("Failed to map material buffer.");
	}
	materialData->color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	materialData->enableLighting = Lambert;
	materialData->uvTransform = Matrix4x4::MakeIdentity();


	/* instance =======================*/
	HRESULT instancingHr = instancingResource_->Map(0, nullptr, reinterpret_cast< void** >(&instancingData));
	if (FAILED(instancingHr)){
		throw std::runtime_error("Failed to map instance buffer.");
	}

	uint32_t index = 0;
	for (auto it = particles_.begin(); it != particles_.end(); ++it, ++index){
		instancingData[index].WVP = Matrix4x4::MakeIdentity();
		instancingData[index].World = Matrix4x4::MakeIdentity();
		instancingData[index].color = it->color;
	}

}

void BaseParticle::CreateSRV(){

	Microsoft::WRL::ComPtr<ID3D12Device>device = GraphicsGroup::GetInstance()->GetDevice();

	auto [srvHandleCPU, srvHandleGPU] = SrvLocator::AllocateSrv();

	D3D12_SHADER_RESOURCE_VIEW_DESC instancingSrvDesc = {};
	instancingSrvDesc.Format = DXGI_FORMAT_UNKNOWN;
	instancingSrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	instancingSrvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	instancingSrvDesc.Buffer.FirstElement = 0;
	instancingSrvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
	instancingSrvDesc.Buffer.NumElements = instanceNum_;
	instancingSrvDesc.Buffer.StructureByteStride = sizeof(ParticleData::ParticleForGPU);

	device->CreateShaderResourceView(instancingResource_.Get(), &instancingSrvDesc, srvHandleCPU);
	instancingSrvHandleGPU_ = srvHandleGPU;

}


void ParticleData::Parameters::SetColorRandom(){

	color = {Random::Generate(0.0f, 1.0f),Random::Generate(0.0f, 1.0f),Random::Generate(0.0f, 1.0f),1.0f};

}

void ParticleData::Parameters::SetColorInitialize(){
	color = Vector4 {1.0f,1.0f,1.0f,1.0f};
}

void ParticleData::Parameters::SetVelocityRandom(float min, float max){
	
	velocity = {Random::Generate(min,max),Random::Generate(min,max),Random::Generate(min,max)};

}

void ParticleData::Emitter::Initialize(uint32_t Count){

	count = Count;
	transform.translate = {0.0f,0.0f,0.0f};
	transform.rotate = {0.0f,0.0f,0.0f};
	transform.scale = {1.0f,1.0f,1.0f};
	frequency = 0.5f;
	frequencyTime = 0.0f;

}

void ParticleData::Emitter::Initialize(const Transform& Transform, const float Frequency, const float FrequencyTime, uint32_t Count){

	count = Count;
	transform = Transform;
	frequency = Frequency;
	frequencyTime = FrequencyTime;

}
