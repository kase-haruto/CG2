#pragma once
/* ========================================================================
/* include space
/* ===================================================================== */
//* engine *//
#include <Engine/Assets/Model/Model.h>
#include <Engine/Assets/Animation/AnimationModel.h>
#include <Engine/Objects/3D/Actor/SceneObject.h>
#include <Engine/objects/Collider/Collider.h>

//* c++ lib *//
#include <memory>
#include <string>
#include <functional>

class BaseGameObject
	:public SceneObject {

	enum ObjectModelType {
		ModelType_Static,		// 静的モデル
		ModelType_Animation,	// アニメーションモデル
		ModelType_Unknown,		// 不明
	};

	enum class ColliderKind{
		None,
		Box,
		Sphere,
	};

public:
	//===================================================================*/
	//                    public methods
	//===================================================================*/
	BaseGameObject(const std::string& modelName);
	BaseGameObject(const std::string& modelName,
				   std::optional<std::string> objectName,
				   std::function<void(IMeshRenderable*)>registerCB);
	BaseGameObject() = default;
	virtual ~BaseGameObject()override;

	virtual void Initialize() {};
	virtual void Update();
	virtual void Draw() {};

	//--------- ui/gui --------------------------------------------------
	void ShowGui()override;
	virtual void DerivativeGui();

	//--------- Collision -----------------------------------------------
	virtual void OnCollisionEnter([[maybe_unused]]Collider* other) {};
	virtual void OnCollisionStay([[maybe_unused]] Collider* other) {};
	virtual void OnCollisionExit([[maybe_unused]] Collider* other){};


	//--------- accessor ------------------------------------------------
	void SetName(const std::string& name);
	void SetTranslate(const Vector3& pos);
	void SetScale(const Vector3& scale);
	virtual const Vector3 GetCenterPos()const;
	void SetColor(const Vector4& color);
	void SetUvScale(const Vector3& uvScale);
	Vector3 GetWorldPosition()const{ return model_->worldTransform_.GetWorldPosition(); }

	void SetCollider(std::unique_ptr<Collider> collider);
	Collider* GetCollider();

private:
	//===================================================================*/
	//                    private methods
	//===================================================================*/
	void SwitchCollider(ColliderKind kind,bool isCollisionEnubled = true);

protected:
	//===================================================================*/
	//                    protected methods
	//===================================================================*/
	std::unique_ptr<BaseModel> model_ = nullptr;					// 描画用モデル
	std::unique_ptr<AnimationModel> animationModel_ = nullptr;		// アニメーションモデル

private:
	//===================================================================*/
	//                    private variables
	//===================================================================*/
	ObjectModelType objectModelType_ = ModelType_Static;

	std::unique_ptr<Collider> collider_;
	ColliderKind currentColliderKind_ = ColliderKind::None;	// コライダーの種類
};
