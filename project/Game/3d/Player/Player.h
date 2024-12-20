#pragma once

#include "../BaseGameObject.h"

#include "Engine/objects/Collider/BoxCollider.h"
#include "Engine/objects/particle/DemoParticle.h"
#include "Engine/physics/Shape.h"

class Player :
	public BaseGameObject, public BoxCollider{

public:
	//===================================================================*/
	//                   public methods
	//===================================================================*/
	Player() = default;
	Player(const std::string& modelName);
	~Player() override;

	void Initialize()override;
	void Update()override;
	void Draw()override;

	/* helper */
	void MoveRight();
	void MoveLeft();

	/* collision ===================================*/
	void OnCollisionEnter(Collider* other)override;
	void OnCollisionStay(Collider* other)override;
	void OnCollisionExit(Collider* other)override;

	void ShowGui();

private:
	//===================================================================*/
	//                   private methods
	//===================================================================*/
	float moveSeed_ = 5.0f;
	std::unique_ptr<DemoParticle> particle_ = nullptr;
};

