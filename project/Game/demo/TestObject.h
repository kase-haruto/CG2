#pragma once
#include "Game/3d/Base/BaseGameObject.h"
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
	void ShowGui()override;

    /* collision ===================================*/
    void OnCollisionEnter(Collider* other)override;
    void OnCollisionStay(Collider* other)override;
    void OnCollisionExit(Collider* other)override;
private:

};

