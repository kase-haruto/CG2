#include "../scene/IScene.h"

/* core */
#include "../core/DirectX/DxCore.h"


IScene::IScene(DxCore* dxCore){
	pDxCore_ = dxCore;
}

IScene::~IScene(){}