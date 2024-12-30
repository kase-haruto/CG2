#include "BaseParticle.h"

//* engine
#include "Engine/core/System.h"
#include "Engine/graphics/GraphicsGroup.h"
#include "Engine/physics/DirectionalLight.h"
#include "Engine/graphics/SrvLocator.h"
#include "Engine/objects/ModelManager.h"
#include "Engine/objects/TextureManager.h"
#include "Engine/graphics/camera/CameraManager.h"
#include "ParticleEmitShape.h"

//* lib
#include "lib/myFunc/MyFunc.h"
#include "lib/myFunc/PrimitiveDrawer.h"
#include "lib/myFunc/Random.h"
#include "lib/myFunc/MathFunc.h"

#include <externals/imgui/imgui.h>

BaseParticle::BaseParticle(){
    particles_.clear();
}

void BaseParticle::Initialize(const std::string& modelName, const std::string& texturePath, const uint32_t count){
    Emit(count);

    modelData_ = ModelManager::LoadModel(modelName);
    textureHandle = TextureManager::GetInstance()->LoadTexture(texturePath);

    backToFrontMatrix_ = MakeRotateYMatrix(std::numbers::pi_v<float>);

    CreateBuffer();
    Map();
    CreateSRV();
}

void BaseParticle::Update(){
    instanceNum_ = 0;
    for (auto it = particles_.begin(); it != particles_.end();){
        if (instanceNum_ < kMaxInstanceNum_){
            if (it->lifeTime <= it->currentTime){
                it = particles_.erase(it);
                continue;
            }

            Matrix4x4 worldMatrix;
            Matrix4x4 worldViewProjectionMatrix;

            if (isBillboard_){
                // ビルボード処理
                Matrix4x4 billboardMatrix = Matrix4x4::Multiply(backToFrontMatrix_, CameraManager::GetCamera3d()->GetWorldMat());
                billboardMatrix.m[3][0] = 0.0f;
                billboardMatrix.m[3][1] = 0.0f;
                billboardMatrix.m[3][2] = 0.0f;

                Matrix4x4 scaleMatrix = MakeScaleMatrix(it->transform.scale);
                Matrix4x4 translateMatrix = MakeTranslateMatrix(it->transform.translate);
                worldMatrix = Matrix4x4::Multiply(Matrix4x4::Multiply(scaleMatrix, billboardMatrix), translateMatrix);
            } else{
                // 通常のスケールとトランスレーション
                Matrix4x4 scaleMatrix = MakeScaleMatrix(it->transform.scale);
                Matrix4x4 translateMatrix = MakeTranslateMatrix(it->transform.translate);
                worldMatrix = Matrix4x4::Multiply(scaleMatrix, translateMatrix);
            }

            // ビルボード有無に関わらず、WVPを計算
            worldViewProjectionMatrix = Matrix4x4::Multiply(worldMatrix, CameraManager::GetViewProjectionMatrix());

            instancingData[instanceNum_].WVP = worldViewProjectionMatrix;
            instancingData[instanceNum_].World = worldMatrix;
            instancingData[instanceNum_].color = it->color;


            float alpha;
            
            if (isFixationAlpha_){
                alpha = 1.0f;
            } else{
                alpha = 1.0f - (it->currentTime / it->lifeTime);
            }

            instancingData[instanceNum_].color.w = alpha;

            it->currentTime += System::GetDeltaTime();
            it->transform.translate += it->velocity * System::GetDeltaTime();

            ++instanceNum_;
        }

        ++it;
    }

    emitter_.frequencyTime += System::GetDeltaTime();
    if (emitter_.frequencyTime >= emitter_.frequency){
        Emit(emitter_.count);
        emitter_.frequencyTime = 0.0f;
    }
}

void BaseParticle::Draw(){
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList = GraphicsGroup::GetInstance()->GetCommandList();

    commandList->IASetVertexBuffers(0, 1, &vertexBufferView);
    commandList->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
    commandList->SetGraphicsRootDescriptorTable(1, instancingSrvHandleGPU_);
    commandList->SetGraphicsRootDescriptorTable(2, textureHandle);

    // DrawInstancedの第2引数で現在のインスタンス数(instanceNum_)を反映
    commandList->DrawInstanced(static_cast< UINT >(modelData_->vertices.size()), instanceNum_, 0, 0);

    // OBB描画
    if (currentShape_ == EmitterShape::OBB){
        PrimitiveDrawer::GetInstance()->DrawOBB(
            emitter_.transform.translate,
            emitter_.transform.rotate,
            emitter_.transform.scale,
            {1.0f,1.0f,1.0f,1.0f}
        );
    } else if (currentShape_ == EmitterShape::Sphere){
        PrimitiveDrawer::GetInstance()->DrawSphere(
            emitter_.transform.translate,
            emitter_.transform.scale.x * 0.5f,
            8,
            {1.0f,1.0f,1.0f,1.0f}
        );
    }

    
}

void BaseParticle::ImGui(){

    if (ImGui::CollapsingHeader("emitterType")){

        // 形状選択UI
        int shapeIndex = ( int ) currentShape_;
        const char* shapeNames[] = {"OBB", "Sphere"};
        if (ImGui::Combo("Emitter Shape", &shapeIndex, shapeNames, IM_ARRAYSIZE(shapeNames))){
            currentShape_ = ( EmitterShape ) shapeIndex;
        }

        if (currentShape_ == EmitterShape::OBB){
            ImGui::DragFloat3("size", &emitter_.transform.scale.x, 0.01f);
            ImGui::Checkbox("Emit +X Face", &emitPosX_);
            ImGui::Checkbox("Emit -X Face", &emitNegX_);
            ImGui::Checkbox("Emit +Y Face", &emitPosY_);
            ImGui::Checkbox("Emit -Y Face", &emitNegY_);
            ImGui::Checkbox("Emit +Z Face", &emitPosZ_);
            ImGui::Checkbox("Emit -Z Face", &emitNegZ_);
        }

        if (currentShape_ == EmitterShape::Sphere){
            ImGui::DragFloat("radius", &emitter_.transform.scale.x, 0.01f);
        }
    }
}

void BaseParticle::Emit(uint32_t count){
    for (uint32_t i = 0; i < count; ++i){
        if (particles_.size() >= static_cast< size_t >(kMaxInstanceNum_)){
            break;
        }

        ParticleData::Parameters particle;
        if (GetUseRandomColor()){
            particle.SetColorRandom();
        } else{
            particle.color = GetSelectedColor();
        }

        float speed = Random::Generate(0.5f, 2.0f);

        Vector3 localPoint {};
        Vector3 localNormal {};
        Vector3 vel {};

        Matrix4x4 rotX = MakeRotateXMatrix(emitter_.transform.rotate.x);
        Matrix4x4 rotY = MakeRotateYMatrix(emitter_.transform.rotate.y);
        Matrix4x4 rotZ = MakeRotateZMatrix(emitter_.transform.rotate.z);
        Matrix4x4 rotationMatrix = Matrix4x4::Multiply(Matrix4x4::Multiply(rotZ, rotY), rotX);

        if (currentShape_ == EmitterShape::OBB){
            FaceInfo fi = GetRandomPointAndNormalOnOBBSurface(emitter_.transform,
                                                              emitPosX_, emitNegX_,
                                                              emitPosY_, emitNegY_,
                                                              emitPosZ_, emitNegZ_);
            localPoint = fi.localPoint;
            localNormal = fi.localNormal;

            Vector3 worldNormal = TransformNormal(localNormal, rotationMatrix);
            particle.velocity = worldNormal * speed;

        } else if (currentShape_ == EmitterShape::Sphere){
            Vector3 si = GetRandomPointOnSphere(emitter_.transform);
            localPoint = si;
            particle.SetVelocityRandom(-1.0f,1.0f);
        }

        Vector3 worldPos = Vector3::Transform(localPoint, rotationMatrix) + emitter_.transform.translate;
        
        particle.transform.translate = worldPos;
        particle.lifeTime = Random::Generate(2.0f, 5.0f);

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

    materialResource_ = CreateBufferResource(device.Get(), sizeof(Material));
    instancingResource_ = CreateBufferResource(device.Get(), sizeof(ParticleData::ParticleForGPU) * kMaxInstanceNum_);
}

void BaseParticle::Map(){
    VertexData* vertexData = nullptr;
    HRESULT hr = vertexBuffer_->Map(0, nullptr, reinterpret_cast< void** >(&vertexData));
    if (FAILED(hr)){
        throw std::runtime_error("Failed to map vertex buffer.");
    }
    std::memcpy(vertexData, modelData_->vertices.data(), sizeof(VertexData) * modelData_->vertices.size());

    HRESULT materialHr = materialResource_->Map(0, nullptr, reinterpret_cast< void** >(&materialData));
    if (FAILED(materialHr)){
        throw std::runtime_error("Failed to map material buffer.");
    }
    materialData->color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
    materialData->enableLighting = Lambert;
    materialData->uvTransform = Matrix4x4::MakeIdentity();

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
    Microsoft::WRL::ComPtr<ID3D12Device> device = GraphicsGroup::GetInstance()->GetDevice();
    auto [srvHandleCPU, srvHandleGPU] = SrvLocator::AllocateSrv();

    D3D12_SHADER_RESOURCE_VIEW_DESC instancingSrvDesc = {};
    instancingSrvDesc.Format = DXGI_FORMAT_UNKNOWN;
    instancingSrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    instancingSrvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
    instancingSrvDesc.Buffer.FirstElement = 0;
    instancingSrvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
    // SRVは常に最大インスタンス数(kMaxInstanceNum_)で作成
    instancingSrvDesc.Buffer.NumElements = kMaxInstanceNum_;
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

void ParticleData::Emitter::Initialize(const EulerTransform& Transform, const float Frequency, const float FrequencyTime, uint32_t Count){

	count = Count;
	transform = Transform;
	frequency = Frequency;
	frequencyTime = FrequencyTime;

}
