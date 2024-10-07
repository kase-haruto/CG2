#include "core/EngineController.h"

void EngineController::Initialize(HINSTANCE hInstance, int width, int height){
    // com�̏�����
    CoInitializeEx(0, COINIT_MULTITHREADED);

    // engine�̏�����
    system_ = std::make_unique<System>();
    system_->Initialize(hInstance, width, height);

    // scene�̏�����
    scene_ = std::make_unique<TestScene>();
    scene_->Initialize();
}

void EngineController::Run(){
    // ���C�����[�v
    while (!system_->ProcessMessage()){

        //�`��O����
        system_->BeginFrame();

        //�V�[���̍X�V
        scene_->Update();

        //�V�[���̕`��
        scene_->Draw();

        //�`��㏈��
        system_->EndFrame();

    }
}

void EngineController::Finalize(){
    //�I������
    scene_->Finalize();
    system_->Finalize();
}