#pragma once

//* engine
#include "Collider.h"

class BoxCollider :
	public Collider{
public:
	//===================================================================*/
	//                   public methods
	//===================================================================*/
	BoxCollider() = default;
	BoxCollider(bool isEnuble);
	~BoxCollider()override = default;

	void Update(const Vector3& position, const Quaternion& rotate)override;
	void Initialize(const Vector3& size);
	void Draw()override;

	void ShowGui()override;


	//* collision ==========================================*//
	void OnCollisionEnter([[maybe_unused]] Collider* other)override{};
	void OnCollisionStay([[maybe_unused]] Collider* other)override{};
	void OnCollisionExit([[maybe_unused]] Collider* other)override{};

protected:
	//===================================================================*/
	//                   private methods
	//===================================================================*/
	OBB shape_; //衝突判定用のobb

private:
	//===================================================================*/
	//                    private methods
	//===================================================================*/
	std::string jsonPath = "gameobject";

public:
	//===================================================================*/
	//                   getter/setter
	//===================================================================*/
	const Vector3& GetCenter()const override;

	const std::variant<Sphere, OBB>& GetCollisionShape() override;
};

