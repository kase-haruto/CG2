#pragma once

//* engine
#include "lib/myMath/Vector3.h"
#include "Engine/physics/Shape.h"

#include <variant>
#include <string>

class Collider{
public:
	//===================================================================*/
	//                   public methods
	//===================================================================*/
	Collider() = default;
	virtual ~Collider() = default;

	virtual void Draw() = 0;

	virtual void OnCollisionEnter([[maybe_unused]]Collider* other){};
	virtual void OnCollisionStay([[maybe_unused]]Collider* other){};
	virtual void OnCollisionExit([[maybe_unused]]Collider* other){};

protected:
	//===================================================================*/
	//                   protected methods
	//===================================================================*/
	std::variant<Sphere, OBB> collisionShape_;
	std::string name_;

public:
	//===================================================================*/
	//                   getter/setter
	//===================================================================*/
	virtual const Vector3& GetCenter()const = 0;
	virtual const std::variant<Sphere, OBB>& GetCollisionShape() = 0;

	const std::string& GetName()const{ return name_; }
	void SetName(const std::string& name){ name_ = name; }
};
