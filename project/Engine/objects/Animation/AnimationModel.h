#pragma once
#include "../Model/BaseModel.h"
#include "AnimationStruct.h"

class AnimationModel
	: public BaseModel{
public:
	//===================================================================*/
	//                   public func
	//===================================================================*/
	AnimationModel() = default;
	AnimationModel(const std::string& fileName);
	~AnimationModel() override = default;

	void Initialize() override;
	void AnimationUpdate() override;
	void Update() override;
	void OnModelLoaded() override;
	void Draw();
	void UpdateMatrix() override;
	void Map() override;
	void ShowImGuiInterface() override;
	void SkeletonUpdate();
	void SkinClusterUpdate();
	void DrawSkeleton();

	//============= 
	// Transform関連
	//=============
	void SetUvScale(const Vector3& uvScale) override{ uvTransform.scale = uvScale; }
	void SetColor(const Vector4& color) override{ RGBa = color; }
	const Vector4& GetColor() const override{ return RGBa; }

	// アニメーション速度の設定と取得
	void SetAnimationSpeed(float speed){ animationSpeed_ = speed; }
	float GetAnimationSpeed() const{ return animationSpeed_; }

	std::vector<std::string> GetAnimationNodeNames() const;

private:
	//===================================================================*/
	//                   private func
	//===================================================================*/
	//============
	// バッファ生成/マップの実装
	//============
	void CreateMaterialBuffer() override;
	void MaterialBufferMap() override;

	// アニメーションを再生する
	void PlayAnimation();

	Quaternion CalculateValue(const AnimationCurve<Quaternion>& curve, float time);
	Vector3 CalculateValue(const AnimationCurve<Vector3>& curve, float time);

private:
	//===================================================================*/
	//                    private variables
	//===================================================================*/
	float animationTime_ = 0.0f;        //< アニメーションの経過時間

	Animation animationData_;               //< アニメーションデータ

	SkinCluster skinCluster_;           //< スキンクラスター
	D3D12_VERTEX_BUFFER_VIEW vbvs_[2]; //< スキンクラスター用のバッファビュー

public:
	float animationSpeed_ = 1.0f;       //< アニメーションの再生速度
};
