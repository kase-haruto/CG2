#include "IScene.h"

/* core */
#include <Engine/core/DirectX/DxCore.h>
#include <Engine/objects/SceneObjectManager.h>


IScene::IScene(){

	//シーンのオブジェクトの初期化
	SceneObjectManager::GetInstance()->ClearAllObject();

}

IScene::IScene(DxCore* dxCore){
	pDxCore_ = dxCore;

}

