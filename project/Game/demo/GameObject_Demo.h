#pragma once

#include "../3d/BaseGameObject.h"

#include "Engine/objects/particle/DemoParticle.h"

class GameObject_Demo :
	public BaseGameObject{

public:
	//===================================================================*/
	//                   public methods
	//===================================================================*/
	GameObject_Demo() = default;
	GameObject_Demo(const std::string& modelName);
	~GameObject_Demo() override = default;

	void Initialize()override;
	void Update()override;
	void Draw()override;
	void ShowDebugUI()override;

private:
	//===================================================================*/
	//                   private methods
	//===================================================================*/

	std::unique_ptr<DemoParticle> particle_ = nullptr;
};

