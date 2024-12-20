#include "SceneObjectManager.h"

SceneObjectManager::SceneObjectManager(){
	allSceneObjects_.clear();
}

SceneObjectManager* SceneObjectManager::GetInstance(){
	static SceneObjectManager instance;
	return &instance;
}

//------------------------------------------------------------------*//
//			リストのクリア
//------------------------------------------------------------------*//
void SceneObjectManager::ClearAllObject(){

	//すべてのオブジェクトを削除
	for (auto& object : allSceneObjects_){
		delete object;
	}
	allSceneObjects_.clear();

}

//------------------------------------------------------------------*//
//			objectの追加
//------------------------------------------------------------------*//
void SceneObjectManager::AddObject(SceneObject* object){
	allSceneObjects_.push_back(object);
}

void SceneObjectManager::Finalize(){
	ClearAllObject();
}