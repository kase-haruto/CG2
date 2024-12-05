#pragma once

#include "../3d/BaseGameObject.h"

#include "Engine/objects/Collider/BoxCollider.h"
#include "Engine/objects/particle/DemoParticle.h"
#include "Engine/physics/Shape.h"

class GameObject_Demo :
	public BaseGameObject,public BoxCollider{

public:
	//===================================================================*/
	//                   public methods
	//===================================================================*/
	GameObject_Demo() = default;
	GameObject_Demo(const std::string& modelName);
	~GameObject_Demo() override;

	void Initialize()override;
	void Update()override;
	void Draw()override;
	void ShowDebugUI()override;

	/* collision ===================================*/
	void OnCollision(Collider* other)override;

private:
	//===================================================================*/
	//                   private methods
	//===================================================================*/

	std::unique_ptr<DemoParticle> particle_ = nullptr;
};

