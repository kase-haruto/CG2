#include "AnimationModel.h"

#include "engine/objects/ModelData.h"
#include "engine/graphics/Material.h"
#include "engine/objects/TransformationMatrix.h"
#include "engine/objects/Transform.h"
#include "lib/myMath/Vector4.h"

#include "engine/graphics/GraphicsGroup.h"
#include "engine/core/System.h"
#include "engine/objects/TextureManager.h"
#include "engine/objects/ModelManager.h"
#include "engine/graphics/VertexData.h"
#include "engine/physics/DirectionalLight.h"
#include "engine/graphics/camera/CameraManager.h"

#ifdef _DEBUG
#include "externals/imgui/imgui.h"
#endif

#include "lib/myfunc/MyFunc.h"

// 追加: アニメーション読み込み関数
extern Animation LoadAnimationFile(const std::string& directoryPath, const std::string& filename);

//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
AnimationModel::AnimationModel(const std::string& fileName){
    Create(fileName);
}

//-----------------------------------------------------------------------------
// モデル生成
//-----------------------------------------------------------------------------
void AnimationModel::Create(const std::string& filename){
    // まずは初期化
    Initialize();

    // モデルデータのロード(既に存在すればキャッシュから取得)
    modelData_ = ModelManager::LoadModel(filename);

    // 頂点バッファとインデックスバッファを取得してビューを作成
    auto vertexResource = ModelManager::GetVertexResource(filename);
    auto indexResource = ModelManager::GetIndexResource(filename);

    vertexBufferView_.BufferLocation = vertexResource->GetGPUVirtualAddress();
    vertexBufferView_.SizeInBytes = static_cast< UINT >(sizeof(VertexData) * modelData_->vertices.size());
    vertexBufferView_.StrideInBytes = sizeof(VertexData);

    indexBufferView_.BufferLocation = indexResource->GetGPUVirtualAddress();
    indexBufferView_.SizeInBytes = static_cast< UINT >(sizeof(uint32_t) * modelData_->indices.size());
    indexBufferView_.Format = DXGI_FORMAT_R32_UINT;

    // テクスチャを読み込む
    handle_ = TextureManager::GetInstance()->LoadTexture(modelData_->material.textureFilePath);

    // ▼ ここでアニメーションを読み込む
    try{
        animation_ = LoadAnimationFile("Resources/models", filename);
    } catch (...){
        // 読み込み失敗時は何もしない or duration=0 のまま
        animation_.duration = 0.0f;
    }
}

//-----------------------------------------------------------------------------
// 初期化
//-----------------------------------------------------------------------------
void AnimationModel::Initialize(){
    // Device, CommandListの取得
    device_ = GraphicsGroup::GetInstance()->GetDevice();
    commandList_ = GraphicsGroup::GetInstance()->GetCommandList();

    // ルートシグネチャ・パイプラインを設定(例: Object3D)
    rootSignature_ = GraphicsGroup::GetInstance()->GetRootSignature(Object3D);
    pipelineState_ = GraphicsGroup::GetInstance()->GetPipelineState(Object3D);

    // デフォルト値
    RGBa = {1.0f, 1.0f, 1.0f, 1.0f};
    transform = {
        {1.0f, 1.0f, 1.0f}, // scale
        {0.0f, 0.0f, 0.0f}, // rotate
        {0.0f, 0.0f, 0.0f}  // translate
    };
    materialParameter_.shininess = 20.0f;
    materialParameter_.enableLighting = HalfLambert;

    // 追加: アニメーション用のtransformを初期化
    animationTransform_ = {
        {1.0f, 1.0f, 1.0f}, // scale
        {0.0f, 0.0f, 0.0f}, // rotate
        {0.0f, 0.0f, 0.0f}  // translate
    };

    // バッファ生成
    CreateMaterialBuffer();
    CreateMatrixBuffer();
    Map();
}

//-----------------------------------------------------------------------------
// アニメーションの再生
//-----------------------------------------------------------------------------
void AnimationModel::PlayAnimation(){
    // duration <= 0 の場合はアニメーションなしとみなす
    if (animation_.duration <= 0.0f){
        return;
    }

    // 時間を進める（アニメーション速度を考慮）
    animationTime_ += System::GetDeltaTime() * animationSpeed_;
    // ループ
    animationTime_ = std::fmod(animationTime_, animation_.duration);

    // 「先頭ノードのアニメーション」を適用
    if (!animation_.nodeAnimations.empty()){
        auto it = animation_.nodeAnimations.begin();
        NodeAnimation& nodeAnimation = it->second;

        // 補間値を取得
        Vector3    newTrans = CalculateValue(nodeAnimation.translate, animationTime_);
        Quaternion newRot = CalculateValue(nodeAnimation.rotate, animationTime_);
        Vector3    newScale = CalculateValue(nodeAnimation.scale, animationTime_);

        // 「アニメーション用」のtransformのみ更新する
        animationTransform_.translate = newTrans;
        animationTransform_.rotate = Quaternion::ToEuler(newRot);
        animationTransform_.scale = newScale;
    }
}


Quaternion AnimationModel::CalculateValue(const AnimationCurve<Quaternion>& curve, float time){
    if (curve.keyframes.empty()){
        return Quaternion(0, 0, 0, 1); // identity
    }
    if (time <= curve.keyframes.front().time){
        return curve.keyframes.front().value;
    }
    if (time >= curve.keyframes.back().time){
        return curve.keyframes.back().value;
    }
    for (int i = 0; i < ( int ) curve.keyframes.size() - 1; ++i){
        float t0 = curve.keyframes[i].time;
        float t1 = curve.keyframes[i + 1].time;
        if (time >= t0 && time <= t1){
            float localT = (time - t0) / (t1 - t0);
            const Quaternion& q0 = curve.keyframes[i].value;
            const Quaternion& q1 = curve.keyframes[i + 1].value;
            return Quaternion::Slerp(q0, q1, localT); // 球面線形補間
        }
    }
    return curve.keyframes.back().value;
}

Vector3 AnimationModel::CalculateValue(const AnimationCurve<Vector3>& curve, float time){
    if (curve.keyframes.empty()){
        return Vector3(0, 0, 0);
    }
    if (time <= curve.keyframes.front().time){
        return curve.keyframes.front().value;
    }
    if (time >= curve.keyframes.back().time){
        return curve.keyframes.back().value;
    }
    for (int i = 0; i < ( int ) curve.keyframes.size() - 1; ++i){
        float t0 = curve.keyframes[i].time;
        float t1 = curve.keyframes[i + 1].time;
        if (time >= t0 && time <= t1){
            float localT = (time - t0) / (t1 - t0);
            const Vector3& v0 = curve.keyframes[i].value;
            const Vector3& v1 = curve.keyframes[i + 1].value;
            return Vector3::Lerp(v0, v1, localT); // 線形補間
        }
    }
    return curve.keyframes.back().value;
}

//-----------------------------------------------------------------------------
// 毎フレームの更新
//-----------------------------------------------------------------------------
void AnimationModel::AnimationUpdate(){
    // (1) アニメーションを再生（animationTransform_ を更新）
    PlayAnimation();

    // (2) UV transform を行列化
    Matrix4x4 uvTransformMatrix = MakeScaleMatrix(uvTransform.scale);
    uvTransformMatrix = Matrix4x4::Multiply(uvTransformMatrix, MakeRotateZMatrix(uvTransform.rotate.z));
    uvTransformMatrix = Matrix4x4::Multiply(uvTransformMatrix, MakeTranslateMatrix(uvTransform.translate));
    materialData_->uvTransform = uvTransformMatrix;

    // (3) マテリアル更新
    materialData_->color = RGBa;
    materialData_->shininess = materialParameter_.shininess;
    materialData_->enableLighting = materialParameter_.enableLighting;

    // (4) ワールド行列の更新
    //   ここで「手動transform + アニメーションtransform」 を合成する
    Matrix4x4 manualMat = MakeAffineMatrix(
        transform.scale,
        transform.rotate,
        transform.translate
    );
    Matrix4x4 animMat = MakeAffineMatrix(
        animationTransform_.scale,
        animationTransform_.rotate,
        animationTransform_.translate
    );

    // 例: 「手動で移動させたオブジェクト」に対して「アニメーションの回転やスケール」を適用
    //     → 最終行列 = manualMat * animMat
    worldMatrix = Matrix4x4::Multiply(animMat,manualMat);

    // (5) カメラ行列との掛け合わせ
    Matrix4x4 worldViewProjectionMatrix =
        Matrix4x4::Multiply(worldMatrix, CameraManager::GetViewProjectionMatrix());

    matrixData_->world = worldMatrix;
    matrixData_->WVP = worldViewProjectionMatrix;
}

void AnimationModel::Update(){
    // 必要に応じて処理を入れる
}

//-----------------------------------------------------------------------------
// 描画
//-----------------------------------------------------------------------------
void AnimationModel::Draw(){
    // ルートシグネチャ・パイプラインをセット
    commandList_->SetGraphicsRootSignature(rootSignature_.Get());
    commandList_->SetPipelineState(pipelineState_.Get());

    // 頂点バッファ/インデックスバッファをセット
    commandList_->IASetVertexBuffers(0, 1, &vertexBufferView_);
    commandList_->IASetIndexBuffer(&indexBufferView_);
    commandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // マテリアル & 行列バッファをセット
    commandList_->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
    commandList_->SetGraphicsRootConstantBufferView(1, wvpResource_->GetGPUVirtualAddress());
    commandList_->SetGraphicsRootDescriptorTable(3, handle_);

    // 描画
    commandList_->DrawIndexedInstanced(UINT(modelData_->indices.size()), 1, 0, 0, 0);
}

//-----------------------------------------------------------------------------
// 行列のみ更新
//-----------------------------------------------------------------------------
void AnimationModel::UpdateMatrix(){
    Matrix4x4 worldViewProjectionMatrix
        = Matrix4x4::Multiply(worldMatrix, CameraManager::GetViewProjectionMatrix());
    matrixData_->world = worldMatrix;
    matrixData_->WVP = worldViewProjectionMatrix;
}

//-----------------------------------------------------------------------------
// ImGui などUIの表示
//-----------------------------------------------------------------------------
void AnimationModel::ShowImGuiInterface(){
#ifdef _DEBUG
    // transform (ユーザーが自由にいじる用)
    if (ImGui::DragFloat3("Translation", &transform.translate.x, 0.01f)){}
    if (ImGui::DragFloat3("Rotation", &transform.rotate.x, 0.01f)){}
    if (ImGui::DragFloat3("Scale", &transform.scale.x, 0.01f)){}
    if (ImGui::DragFloat3("uvScale", &uvTransform.scale.x, 0.01f)){}

    ImGui::DragFloat("shininess", &materialData_->shininess, 0.01f);

    // ライティングモードなど
    const char* lightingModes[] = {"Half-Lambert", "Lambert", "SpecularReflection", "No Lighting"};
    if (ImGui::BeginCombo("Lighting Mode", lightingModes[currentLightingMode_])){
        for (int n = 0; n < IM_ARRAYSIZE(lightingModes); n++){
            bool is_selected = (currentLightingMode_ == n);
            if (ImGui::Selectable(lightingModes[n], is_selected)){
                currentLightingMode_ = n;
                materialData_->enableLighting = currentLightingMode_;
            }
            if (is_selected){
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }

    // もし複数ノードを選択可能にしたいなら、以下のようにnodeAnimationsのキー一覧をUI表示など
    // for (auto &pair : animation_.nodeAnimations) {
    //     ImGui::Text("Node: %s", pair.first.c_str());
    // }

    // アニメーション用transformを直接UI操作したい場合は下記のように追加してもOK
    // if (ImGui::TreeNode("AnimationTransform")) {
    //     ImGui::DragFloat3("Anim Translation", &animationTransform_.translate.x, 0.01f);
    //     ImGui::DragFloat3("Anim Rotation",    &animationTransform_.rotate.x,    0.01f);
    //     ImGui::DragFloat3("Anim Scale",       &animationTransform_.scale.x,     0.01f);
    //     ImGui::TreePop();
    // }

#endif
}

//-----------------------------------------------------------------------------
// バッファ生成/マッピング
//-----------------------------------------------------------------------------
void AnimationModel::Map(){
    MaterialBufferMap();
    MatrixBufferMap();
}

void AnimationModel::CreateMaterialBuffer(){
    materialResource_ = CreateBufferResource(device_.Get(), sizeof(Material));
}

void AnimationModel::CreateMatrixBuffer(){
    wvpResource_ = CreateBufferResource(device_.Get(), sizeof(TransformationMatrix));
}

void AnimationModel::MaterialBufferMap(){
    materialResource_->Map(0, nullptr, reinterpret_cast< void** >(&materialData_));
    materialData_->color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
    materialData_->enableLighting = HalfLambert;
    materialData_->uvTransform = Matrix4x4::MakeIdentity();
    materialData_->shininess = 20.0f;
    materialResource_->Unmap(0, nullptr);
}

void AnimationModel::MatrixBufferMap(){
    wvpResource_->Map(0, nullptr, reinterpret_cast< void** >(&matrixData_));
    matrixData_->WVP = Matrix4x4::MakeIdentity();
    matrixData_->world = Matrix4x4::MakeIdentity();
    wvpResource_->Unmap(0, nullptr);
}

//-----------------------------------------------------------------------------
// ノード名の取得例
//-----------------------------------------------------------------------------
std::vector<std::string> AnimationModel::GetAnimationNodeNames() const{
    std::vector<std::string> names;
    for (auto& pair : animation_.nodeAnimations){
        names.push_back(pair.first);
    }
    return names;
}
