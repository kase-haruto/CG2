// IPlayerAttack.h
#pragma once

#include "Engine/objects/Collider/BoxCollider.h"
#include "lib/myMath/Vector3.h"
#include "../Weapon/Weapon.h"

#include <string>
#include <memory>
#include <vector>

class Player;

class IPlayerAttack : public BoxCollider{
public:
	IPlayerAttack(const std::string& attackName);
	virtual ~IPlayerAttack();

	virtual void Initialize();    //< 初期化
	virtual void Execution() = 0;     //< 実行
	virtual void Update() = 0;         //< 更新
	virtual void Draw() = 0;           //< 描画
	virtual void ShowGui() = 0;        //< GUI表示
	virtual void Cleanup(){};        //< クリーンアップ

	/* collision ===================================*/
	virtual void OnCollisionEnter([[maybe_unused]] Collider* other) override{}
	virtual void OnCollisionStay([[maybe_unused]] Collider* other) override{}
	virtual void OnCollisionExit([[maybe_unused]] Collider* other) override{}

	// クローン作成メソッド
	virtual std::unique_ptr<IPlayerAttack> Clone() const = 0;

	// コピーコンストラクタの宣言
	IPlayerAttack(const IPlayerAttack& other);

public:
	//===================================================================*/
	//					getter/setter
	//===================================================================*/
	void SetCenter(const Vector3& center){ center_ = center; }

	// 攻撃中かどうかを取得
	bool IsAttacking() const{ return isAttacking_; }

	void SetIsActive(bool isActive){
		isActive_ = isActive;
	}

	// 攻撃名の取得
	const std::string& GetName() const{ return attackName_; }

	// damageの取得
	int GetDamage() const{ return damage_; }

	// Weaponのセット
	void SetWeapon(Weapon* weapon){ weapon_ = weapon; }
	void SetPlayer(Player* pPlayer);

	const Vector3 GetPlayerPos() const;

	// 制御点の取得
	virtual const std::vector<Vector3>& GetControlPoints() const = 0;
	// 攻撃タイプの取得
	virtual std::string GetType() const = 0;

	virtual void SetControlPoints(const std::vector<Vector3>& controlPoints) = 0;

	// **コライダーアクセサの実装**
	Vector3 GetColliderCenter() const{ return shape_.center; }
	void SetColliderCenter(const Vector3& center){ shape_.center = center; }

	Vector3 GetColliderSize() const{ return shape_.size; }
	void SetColliderSize(const Vector3& size){ shape_.size = size; }


protected:
	Vector3 center_;            //< 衝突判定用の中心座標
	float offset_;				//< 衝突判定用のオフセット
	Vector3 rotate_;            //< 衝突判定用の回転角度

	bool isAttacking_ = false;  //< 攻撃中フラグ

	Weapon* weapon_ = nullptr;  //< Weaponへのポインタ
	Player* pPlayer_ = nullptr; //< Playerへのポインタ

	int damage_ = 1;            //< ダメージ量

private:
	std::string attackName_;    //< 攻撃名
};
