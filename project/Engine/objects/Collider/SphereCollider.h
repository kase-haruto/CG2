#pragma once

//* engine
#include "Collider.h"

class SphereCollider :
    public Collider{
public:
    //===================================================================*/
    //                   public methods
    //===================================================================*/
    SphereCollider() = default;
    ~SphereCollider()override = default;

    void Initialize(float radius);
    void Draw()override;

protected:
    //===================================================================*/
    //                   private methods
    //===================================================================*/
    Sphere shape_; //衝突判定用のobb

public:
    //===================================================================*/
    //                   getter/setter
    //===================================================================*/
    const Vector3& GetCenter()const override;

    const std::variant<Sphere, OBB>& GetCollisionShape() override;
};
