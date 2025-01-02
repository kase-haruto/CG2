#pragma once

#include "Engine/objects/Collider/BoxCollider.h"
#include "lib/myMath/Vector3.h"
#include "Game/3d/Player/Weapon/Weapon.h"

#include <string>
#include <memory>

class IPlayerAttack : public BoxCollider{
public:
    //===================================================================*/
    //					public member functions
    //===================================================================*/
    IPlayerAttack(const std::string& attackName);
    virtual ~IPlayerAttack();

    virtual void Initialize() = 0;	//< 初期化
    virtual void Execution() = 0;	//< 実行
    virtual void Update() = 0;		//< 更新
    virtual void Draw() = 0;		//< 描画
    virtual void ShowGui() = 0;		//< GUI表示

    /* collision ===================================*/
    virtual void OnCollisionEnter([[maybe_unused]] Collider* other) override{}
    virtual void OnCollisionStay([[maybe_unused]] Collider* other) override{}
    virtual void OnCollisionExit([[maybe_unused]] Collider* other) override{}

    // クローン作成メソッド
    virtual std::unique_ptr<IPlayerAttack> Clone() const = 0;

protected:
    //===================================================================*/
    //					protected member variables
    //===================================================================*/
    Vector3 center_;			//< 衝突判定用の中心座標
    Vector3 offset_;			//< 衝突判定用のオフセット
    Vector3 rotate_;			//< 衝突判定用の回転角度

    bool isAttacking_ = false;	//< 攻撃中フラグ

	Weapon* weapon_ = nullptr;	//< 武器

public:
    //===================================================================*/
    //					getter/setter
    //===================================================================*/
    void SetCenter(const Vector3& center){ center_ = center; }
    // 攻撃中かどうかを取得
    bool IsAttacking() const{ return isAttacking_; }
    // 攻撃名の取得
    const std::string& GetName() const{ return attackName_; }

	void SetWeapon(Weapon* weapon){ weapon_ = weapon; }

private:
    std::string attackName_; //< 攻撃名
};
