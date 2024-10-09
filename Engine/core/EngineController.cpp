#include "core/EngineController.h"
#include "core/Enviroment.h"

void EngineController::Initialize(HINSTANCE hInstance){
    // comの初期化
    CoInitializeEx(0, COINIT_MULTITHREADED);

    // engineの初期化
    system_ = std::make_unique<System>();
    system_->Initialize(hInstance, kWindowWidth, kWindowHeight, windowTitle);

    // sceneの初期化
    scene_ = std::make_unique<TestScene>();
    scene_->Initialize();
}

void EngineController::Run(){
    // メインループ
    while (!system_->ProcessMessage()){

        //描画前処理
        system_->BeginFrame();

        //シーンの更新
        scene_->Update();

        //シーンの描画
        scene_->Draw();

        //描画後処理
        system_->EndFrame();

    }
}

void EngineController::Finalize(){
    //終了処理
    scene_->Finalize();
    system_->Finalize();
}