#pragma once

#include "Engine/objects/SceneObject.h"

#include <vector>

class SceneObjectManager{
public:
	//==================================================================*//
	//			singleton
	//==================================================================*//
	//コピー参照禁止
	SceneObjectManager(const SceneObjectManager&) = delete;
	SceneObjectManager& operator=(const SceneObjectManager&) = delete;
	//ムーブ参照禁止
	SceneObjectManager(SceneObjectManager&&) = delete;
	SceneObjectManager& operator=(SceneObjectManager&&) = delete;
	//インスタンス取得
	static SceneObjectManager* GetInstance();

public:
	//==================================================================*//
	//			public functions
	//==================================================================*//
	void ClearAllObject();	//全てのオブジェクトを削除
	void Finalize();		//終了処理

	//objectの取得追加
	void AddObject(SceneObject* object);
	const std::vector<SceneObject*>& GetAllObjects()const{ return allSceneObjects_; }

private:
	//==================================================================*//
	//			private functions
	//==================================================================*//
	SceneObjectManager();				//コンストラクタ
	~SceneObjectManager() = default;	//デストラクタ

private:
	//==================================================================*//
	//			private variables
	//==================================================================*//
	std::vector<SceneObject*> allSceneObjects_;	//全てのオブジェクト
};

