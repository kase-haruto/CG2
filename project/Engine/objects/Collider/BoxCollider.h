#pragma once

//* engine
#include "Collider.h"

class BoxCollider :
    public Collider{
public:
    //===================================================================*/
    //                   public methods
    //===================================================================*/
    BoxCollider() = default;
    ~BoxCollider()override = default;

    void Initialize(const Vector3& size);
    void Draw()override;

protected:
    //===================================================================*/
    //                   private methods
    //===================================================================*/
    OBB shape_; //衝突判定用のobb

public:
    //===================================================================*/
    //                   getter/setter
    //===================================================================*/
    const Vector3& GetCenter()const override;

    const std::variant<Sphere, OBB>& GetCollisionShape() override;
};

