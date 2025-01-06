#pragma once

#include "../Base/Character.h"
#include "Engine/objects/Collider/BoxCollider.h"
#include "Engine/objects/particle/DemoParticle.h"
#include "Engine/physics/Shape.h"
#include "../Player/PlayerParticle/AttackParticle.h"

class Enemy:
public Character,public BoxCollider{

public:
	//===================================================================*/
	//                   public methods
	//===================================================================*/
	Enemy() = default;
	Enemy(const std::string& modelName);
	~Enemy() override;

	void Initialize()override;
	void Update()override;
	void Draw()override;
	
	/* collision ===================================*/
	void OnCollisionEnter(Collider* other)override;
	void OnCollisionStay(Collider* other)override;
	void OnCollisionExit(Collider* other)override;
	
	/* ui =========================================*/
	void ShowGui();

private:
	//===================================================================*/
	//                   private methods
	//===================================================================*/
	std::unique_ptr<AttackParticle> hitParticle_ = nullptr;
};