#pragma once

//* engine
#include <Engine/Foundation/Math/Vector3.h>
#include <Engine/Objects/3D/Geometory/Shape.h>

#include <variant>
#include <string>

//===================================================================*/
//							  ColliderType
//===================================================================*/
enum class ColliderType{
	Type_None				= 0,        // ビットが立っていない状態
	Type_Player				= 1 << 1,
	Type_PlayerAttack		= 2 << 1,
	Type_Enemy				= 3 << 1,
	Type_EnemySpawner		= 4 << 1
};

// ビット演算のオーバーロード
inline ColliderType operator|(ColliderType lhs, ColliderType rhs){
	using T = std::underlying_type_t<ColliderType>;
	return static_cast< ColliderType >(static_cast< T >(lhs) | static_cast< T >(rhs));
}

inline ColliderType& operator|=(ColliderType& lhs, ColliderType rhs){
	lhs = lhs | rhs;
	return lhs;
}

// ビットAND演算のオーバーロード
inline ColliderType operator&(ColliderType lhs, ColliderType rhs){
	using T = std::underlying_type_t<ColliderType>;
	return static_cast< ColliderType >(static_cast< T >(lhs) & static_cast< T >(rhs));
}

// ビットAND代入演算のオーバーロード
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
	Collider(bool isEnuble);
	virtual ~Collider() = default;
	virtual void Update(const Vector3& position,const Quaternion& rotate) = 0;
	virtual void Draw() = 0;
	virtual void ShowGui();

	virtual void OnCollisionEnter([[maybe_unused]] Collider* other){};
	virtual void OnCollisionStay([[maybe_unused]] Collider* other){};
	virtual void OnCollisionExit([[maybe_unused]] Collider* other){};

protected:
	//===================================================================*/
	//                   protected methods
	//===================================================================*/
	std::variant<Sphere, OBB> collisionShape_;
	std::string name_;

	ColliderType type_;							//< 自身のタイプ
	ColliderType targetType_;					//< 衝突相手のタイプ
	Vector4 color_ = {1.0, 0.0, 0.0, 1.0};		//< 描画色

	bool isCollisionEnabled_ = true;				//< 衝突判定を行うかどうか
	bool isDraw_ = true;						//< 描画を行うかどうか
	bool isTrigger_ = false;					//< 押し戻しなどを行うかどうか
public:
	//===================================================================*/
	//                   getter/setter
	//===================================================================*/
	virtual const Vector3& GetCenter()const = 0;
	virtual const std::variant<Sphere, OBB>& GetCollisionShape() = 0;

	const std::string& GetName()const{ return name_; }
	void SetName(const std::string& name){ name_ = name; }

	ColliderType GetType() const{ return type_; }
	ColliderType GetTargetType() const{ return targetType_; }
	void SetColor(const Vector4& color){ color_ = color; }

	bool IsCollisionEnubled()const{ return isCollisionEnabled_; }
	void SetCollisionEnabled(bool isCollisionEnuble);

	void SetIsDrawCollider(bool isDraw) { isDraw_ = isDraw; }
};


