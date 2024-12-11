#pragma once

#include "../BaseGameObject.h"
#include "Engine/objects/Collider/BoxCollider.h"

class Weapon :
    public BaseGameObject, public BoxCollider{
public:
	//===================================================================*/
	//                   public methods
	//===================================================================*/
	Weapon() = default;
	Weapon(const std::string& modelName);
	~Weapon()override;

	void Initialize()override;
	void Update()override;
	void Draw()override;
	void ImGui()override;
	void UpdateWorldMat();
	
	/* collision ============================================================ */
	void OnCollisionEnter(Collider* other)override;
	void OnCollisionStay(Collider* other)override;
	void OnCollisionExit(Collider* other)override;

private:
	//===================================================================*/
	//                   private methods
	//===================================================================*/
	bool isAttacking_ = false;

public:
	//===================================================================*/
	//                   getter/setter
	//===================================================================*/
	void SetCollisionSpace(const Vector3& position, const Vector3& size);
	void SetIsAttacking(bool flag){ isAttacking_ = flag; }

	void SetWorldMat(const Matrix4x4& worldMat){ model_->worldMatrix = worldMat; }
	const Matrix4x4& GetWorldMat()const{ return model_->worldMatrix; }
};

