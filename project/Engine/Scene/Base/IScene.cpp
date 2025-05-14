#include "IScene.h"

/* core */
#include <Engine/Graphics/Device/DxCore.h>
#include <Engine/Objects/3D/Actor/SceneObjectManager.h>


IScene::IScene(){

	//シーンのオブジェクトの初期化
	SceneObjectManager::GetInstance()->ClearAllObject();

}

IScene::IScene(DxCore* dxCore){
	pDxCore_ = dxCore;

}

