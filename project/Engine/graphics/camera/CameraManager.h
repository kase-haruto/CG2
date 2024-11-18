#pragma once

#include "Engine/graphics/camera/Camera3d.h"

//* c++ *//
#include <memory>

class CameraManager{
public:
    // インスタンスを取得する関数
    static CameraManager* GetInstance();

    // コピーコンストラクタと代入演算子を削除して、複製を防止
    CameraManager(const CameraManager&) = delete;
    CameraManager& operator=(const CameraManager&) = delete;

public:
    //===================================================================*/
    //					private member function
    //===================================================================*/
    static void Initialize();      // 初期化
    static void Update();          // 更新
    static void Finalize();        // 解放

private:
    CameraManager();
    ~CameraManager() = default;

private:
   //===================================================================*/
   //					private member variable
   //===================================================================*/
   static CameraManager* instance_;                 // クラスのインスタンス


   /* 管理しているカメラ =======================*/
   std::unique_ptr<Camera3d> camera3d_ = nullptr;   // 3dオブジェクトのデフォルトカメラ


   //===================================================================*/
   //					getter/setter
   //===================================================================*/
public:
   static Camera3d* GetCamera3d() { return instance_->camera3d_.get(); }
};
