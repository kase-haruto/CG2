#include "SceneObject.h"
#include <externals/imgui/imgui.h>
#include "SceneObjectManager.h"

SceneObject::SceneObject(){

	//リストに追加
	SceneObjectManager::GetInstance()->AddObject(this);

}
