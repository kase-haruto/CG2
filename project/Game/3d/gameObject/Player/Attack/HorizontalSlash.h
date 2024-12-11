#pragma once
#include "IPlayerAttack.h"
#include "Engine/objects/particle/DemoParticle.h"

class HorizontalSlash :
    public IPlayerAttack{
public:
	//===================================================================*/
	//                    public methods
	//===================================================================*/
	HorizontalSlash();
	~HorizontalSlash() override = default;

	void Initialize()override;
	void Execution()override;
	void Draw()override;

private:
	Vector3 startPos_;
	float currentRotationAngle_;
};

