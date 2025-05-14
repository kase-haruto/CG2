#pragma once
/* ========================================================================
/* include space
/* ===================================================================== */
//* engine *//
#include <Engine/Assets/Model/Model.h>
#include <Engine/Assets/Animation/AnimationModel.h>
#include <Engine/Objects/3D/Actor/SceneObject.h>

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

	virtual void Initialize();
	virtual void Update();
	virtual void Draw();
	void ShowGui()override;
	virtual void DerivativeGui();

	void SaveToJson(const std::string& fileName)const;
	void LoadFromJson(const std::string& fileName);

protected:
	//===================================================================*/
	//                    protected methods
	//===================================================================*/
	std::unique_ptr<BaseModel> model_ = nullptr;					// 描画用モデル
	std::unique_ptr<AnimationModel> animationModel_ = nullptr;		// アニメーションモデル

protected:
	//===================================================================*/
	//                    private methods
	//===================================================================*/
	std::string jsonPath = "gameObjects";

private:
	//===================================================================*/
	//                    private variables
	//===================================================================*/
	ObjectModelType objectModelType_ = ModelType_Static;

public:
	//===================================================================*/
	//                    getter/setter
	//===================================================================*/
	void SetName(const std::string& name);
	const std::string& GetName()const { return name_; }

	void SetTranslate(const Vector3& pos) {
		if (model_) {
			model_->worldTransform_.translation = pos;
		}
	}

	void SetScale(const Vector3& scale) {
		if (model_) {
			model_->worldTransform_.scale = scale;
		}
	}

	virtual const Vector3 GetCenterPos()const {
		const Vector3 offset = { 0.0f, 1.0f, 0.0f };
		Vector3 worldPos = Vector3::Transform(offset, model_->GetWorldTransform().matrix.world);
		return worldPos;
	}
	void SetColor(const Vector4& color) {
		if (model_) {
			model_->SetColor(color);
		}
	}
	BaseModel* GetModel()const { return model_.get(); }
};
