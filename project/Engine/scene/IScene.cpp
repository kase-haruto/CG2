#include "../scene/IScene.h"

/* core */
#include "../core/DirectX/DxCore.h"
#include "Engine/objects/SceneObjectManager.h"


IScene::IScene(){

	//シーンのオブジェクトの初期化
	SceneObjectManager::GetInstance()->ClearAllObject();

}

IScene::IScene(DxCore* dxCore){
	pDxCore_ = dxCore;

	//シーンのオブジェクトの初期化
	SceneObjectManager::GetInstance()->ClearAllObject();
}

