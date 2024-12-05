#pragma once

//* engine
#include "lib/myMath/Vector3.h"
#include "Engine/physics/Shape.h"

#include <variant>

class Collider{
public:
	//===================================================================*/
	//                   public methods
	//===================================================================*/
	Collider() = default;
	virtual ~Collider() = default;

	virtual void Draw() = 0;

	virtual void OnCollision([[maybe_unused]]Collider* other){};

protected:
	//===================================================================*/
	//                   protected methods
	//===================================================================*/
	std::variant<Sphere, OBB> collisionShape_;

public:
	//===================================================================*/
	//                   getter/setter
	//===================================================================*/
	virtual const Vector3& GetCenter()const = 0;
	virtual const std::variant<Sphere, OBB>& GetCollisionShape() = 0;
};
