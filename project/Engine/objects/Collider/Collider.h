#pragma once

//* engine
#include "lib/myMath/Vector3.h"
#include "Engine/physics/Shape.h"

#include <variant>
#include <string>

//===================================================================*/
//							  ColliderType
//===================================================================*/
enum class ColliderType{
	Type_None			= 0	,	  // �r�b�g�������Ă��Ȃ����
	Type_Player			= 1 << 1,
	Type_PlayerAttack	= 1 << 2,
	Type_Enemy			= 1 << 3,
};

// �r�b�g���Z�̃I�[�o�[���[�h
inline ColliderType operator|(ColliderType lhs, ColliderType rhs){
	using T = std::underlying_type_t<ColliderType>;
	return static_cast< ColliderType >(static_cast< T >(lhs) | static_cast< T >(rhs));
}

inline ColliderType& operator|=(ColliderType& lhs, ColliderType rhs){
	lhs = lhs | rhs;
	return lhs;
}

// �r�b�gAND���Z�̃I�[�o�[���[�h
inline ColliderType operator&(ColliderType lhs, ColliderType rhs){
	using T = std::underlying_type_t<ColliderType>;
	return static_cast< ColliderType >(static_cast< T >(lhs) & static_cast< T >(rhs));
}

// �r�b�gAND������Z�̃I�[�o�[���[�h
inline ColliderType& operator&=(ColliderType& lhs, ColliderType rhs){
	lhs = lhs & rhs;
	return lhs;
}

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

	ColliderType colliderType_;			//����^�C�v
	ColliderType targetColliderType_;	//�Փˑ���̃^�C�v

public:
	//===================================================================*/
	//                   getter/setter
	//===================================================================*/
	virtual const Vector3& GetCenter()const = 0;
	virtual const std::variant<Sphere, OBB>& GetCollisionShape() = 0;

	const std::string& GetName()const{ return name_; }
	void SetName(const std::string& name){ name_ = name; }

	ColliderType GetType()const{ return colliderType_; }
	ColliderType GetTargetType()const{ return targetColliderType_; }
};
