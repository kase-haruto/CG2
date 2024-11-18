#include "ParticleManager.h"
#include "Engine/graphics/VertexData.h"
#include "engine/objects/TextureManager.h"
#include "engine/graphics/GraphicsGroup.h"
#include "engine/physics/DirectionalLight.h"
#include "engine/graphics/SrvLocator.h"
#include "engine/objects/ModelManager.h"
#include "engine/graphics/camera/CameraManager.h"
#include "lib/myfunc/MyFunc.h"

#ifdef _DEBUG
#include <externals/imgui/imgui.h>
#endif

#include <string>
#include <numbers>
#include <random>

// コンストラクタ
ParticleManager::ParticleManager(uint32_t maxInstances)
    : maxInstances_(maxInstances){}

// デストラクタ
ParticleManager::~ParticleManager(){}

// 初期化処理
void ParticleManager::Initialize(){
    device_ = GraphicsGroup::GetInstance()->GetDevice();
    commandList_ = GraphicsGroup::GetInstance()->GetCommandList();
    rootSignature_ = GraphicsGroup::GetInstance()->GetRootSignature(StructuredObject);
    pipelineState_ = GraphicsGroup::GetInstance()->GetPipelineState(StructuredObject);

    baseColor_ = {1.0f, 1.0f, 1.0f, 1.0f};

    emitter_.transform.translate = {0.0f, 0.0f, 0.0f};
    emitter_.frequency = 0.5f;
    emitter_.count = 5;

    // モデルデータの読み込み
    modelData_ = ModelManager::LoadModel("plane");
    if (!modelData_){
        throw std::runtime_error("Failed to load model data.");
    }

    // バッファの作成
    CreateBuffer();
    MapResources();
}


// パーティクル生成
void ParticleManager::Create(){
    std::random_device rd;
    std::mt19937 randomEngine(rd());

    for (uint32_t i = 0; i < maxInstances_; ++i){
        particles_.emplace_back(MakeNewParticle(randomEngine, emitter_.transform.translate));
    }
}

// 新しいパーティクルの生成
Particle ParticleManager::MakeNewParticle(std::mt19937& randomEngine, const Vector3& translate){
    Particle newParticle;
    std::uniform_real_distribution<float> dist(-1.0f, 1.0f);
    std::uniform_real_distribution<float> lifetimeDist(1.0f, 3.0f);

    newParticle.transform.translate = translate + Vector3(dist(randomEngine), dist(randomEngine), dist(randomEngine));
    newParticle.velocity = Vector3(dist(randomEngine), dist(randomEngine), dist(randomEngine));
    newParticle.color = baseColor_;
    newParticle.lifeTime = lifetimeDist(randomEngine);
    newParticle.currentTime = 0.0f;

    return newParticle;
}


// UIの更新（ImGui）
void ParticleManager::UpdateUI(){
#ifdef _DEBUG
    ImGui::Begin("Particle Manager");

    // エミッター設定
    ImGui::Text("Emitter Settings:");
    ImGui::DragFloat3("Emitter Position", &emitter_.transform.translate.x, 0.1f, -100.0f, 100.0f);
    ImGui::DragFloat("Frequency", &emitter_.frequency, 0.01f, 0.1f, 10.0f);
    ImGui::SliderInt("Particle Count", reinterpret_cast< int* >(&emitter_.count), 1, 100);

    // カラー設定
    ImGui::Text("Particle Color:");
    ImGui::ColorEdit4("Base Color", &baseColor_.x);

    // パーティクル個別設定
    ImGui::Text("Particle Settings:");
    for (auto& particle : particles_){
        ImGui::PushID(&particle);
        ImGui::DragFloat3("Position", &particle.transform.translate.x, 0.1f, -100.0f, 100.0f);
        ImGui::DragFloat3("Velocity", &particle.velocity.x, 0.1f, -50.0f, 50.0f);
        ImGui::SliderFloat("Lifetime", &particle.lifeTime, 0.1f, 10.0f);
        ImGui::PopID();
    }

    ImGui::End();
#endif
}


// パーティクルの更新
void ParticleManager::Update(){
    std::random_device rd;
    std::mt19937 randomEngine(rd());

#ifdef _DEBUG
    UpdateUI();
#endif

    numActiveInstances_ = 0;

    for (auto it = particles_.begin(); it != particles_.end();){
        if (it->currentTime > it->lifeTime){
            it = particles_.erase(it);
        } else{
            // パーティクルごとの更新処理
            for (auto& behavior : behaviors_){
                behavior->Update(*it, deltaTime);
            }

            // パーティクルの位置更新
            it->transform.translate += it->velocity * deltaTime;
            it->currentTime += deltaTime;
            ++numActiveInstances_;
            ++it;
        }
    }

    // エミッターから新しいパーティクルを生成
    emitter_.elapsedTime += deltaTime;
    if (emitter_.elapsedTime >= emitter_.frequency){
        particles_.splice(particles_.end(), Emit(emitter_, randomEngine));
        emitter_.elapsedTime -= emitter_.frequency;
    }
}
// 描画処理
void ParticleManager::Draw(){
    commandList_->SetGraphicsRootSignature(rootSignature_.Get());
    commandList_->SetPipelineState(pipelineState_.Get());
    commandList_->IASetVertexBuffers(0, 1, &vertexBufferView_);
    commandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // マテリアルバッファ
    commandList_->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());

    // テクスチャ (最初のテクスチャを使用)
    auto it = textures_.begin();
    if (it != textures_.end()){
        commandList_->SetGraphicsRootDescriptorTable(1, it->second);
    }

    // 描画
    commandList_->DrawInstanced(static_cast< UINT >(modelData_->vertices.size()), numActiveInstances_, 0, 0);
}
// 挙動を追加
void ParticleManager::AddBehavior(std::unique_ptr<ParticleBehavior> behavior){
    behaviors_.emplace_back(std::move(behavior));
}

// エミッターからパーティクルを生成
std::list<Particle> ParticleManager::Emit(const Emitter& emitter, std::mt19937& randomEngine){
    std::list<Particle> emittedParticles;

    for (uint32_t i = 0; i < emitter.count; ++i){
        emittedParticles.emplace_back(MakeNewParticle(randomEngine, emitter.transform.translate));
    }

    return emittedParticles;
}

// バッファ作成
void ParticleManager::CreateBuffer(){
    CreateVertexBuffer();
    CreateMaterialBuffer();
    CreateMatrixBuffer();
}

// 各種バッファの作成
void ParticleManager::CreateVertexBuffer(){
    vertexResource_ = CreateBufferResource(device_, sizeof(VertexData) * maxInstances_);
    vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
    vertexBufferView_.SizeInBytes = sizeof(VertexData) * maxInstances_;
    vertexBufferView_.StrideInBytes = sizeof(VertexData);
}

void ParticleManager::CreateMaterialBuffer(){
    materialResource_ = CreateBufferResource(device_, sizeof(Material));
}

void ParticleManager::CreateMatrixBuffer(){
    instancingResource_ = CreateBufferResource(device_, sizeof(ParticleForGPU) * maxInstances_);
}

// リソースのマッピング
void ParticleManager::MapResources(){
    MapVertexBuffer();
    MapMaterialBuffer();
    MapMatrixBuffer();
}

void ParticleManager::MapVertexBuffer(){
    VertexData* vertexData = nullptr;
    HRESULT hr = vertexResource_->Map(0, nullptr, reinterpret_cast< void** >(&vertexData));
    if (FAILED(hr)){
        throw std::runtime_error("Failed to map vertex buffer.");
    }
}

void ParticleManager::MapMaterialBuffer(){
    HRESULT hr = materialResource_->Map(0, nullptr, reinterpret_cast< void** >(&materialData_));
    if (FAILED(hr)){
        throw std::runtime_error("Failed to map material buffer.");
    }
}

void ParticleManager::MapMatrixBuffer(){
    HRESULT hr = instancingResource_->Map(0, nullptr, reinterpret_cast< void** >(&instancingData_));
    if (FAILED(hr)){
        throw std::runtime_error("Failed to map instance buffer.");
    }
}
