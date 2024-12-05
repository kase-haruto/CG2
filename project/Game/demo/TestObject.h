#pragma once
#include "Game/3d/BaseGameObject.h"
#include "Engine/objects/Collider/SphereCollider.h"
class TestObject :
    public BaseGameObject,public SphereCollider{
public:
    //===================================================================*/
    //                   public methods
    //===================================================================*/
    TestObject() = default;
    TestObject(const std::string& modelName);
    ~TestObject()override;

    void Initialize()override;
    void Update()override;
    void Draw()override;
    void ShowDebugUI()override;

    /* collision ===============================================*/
    void OnCollision(Collider* other)override;

private:

};

