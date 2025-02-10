#pragma once

#include "BaseParticle.h"

#include <string>
#include "particleBehavior/ParticleBehavior.h"

class ParticleSystem 
    : public BaseParticle{
protected:
    // カラーモード用のenumを定義
    enum class ColorMode{
        Random,
        SingleColor,
        SimilarColor
    };

public:
    //===================================================================*/
    //                    public methods
    //===================================================================*/

    ParticleSystem();
    ~ParticleSystem() override;

    virtual void Initialize(const std::string& modelName, const std::string& texturePath);

    /* ui ===============================================*/
    virtual void ImGui();
    void Load(const std::string& filename);
    void Save(const std::string& filename);

protected:
    //===================================================================*/
    //                    private methods
    //===================================================================*/
    std::string name_;                                  // システム名
    bool useRandomColor_ = true;                        // ランダムカラーを使用するか
    Vector4 selectedColor_ = {1.0f, 1.0f, 1.0f, 1.0f};  // ランダムでない場合に使う色

    std::string fileDirectoryPath = "./Resources/json/particle/";

    ColorMode colorMode_ = ColorMode::Random; // 現在のカラー方式
    //Vector4 selectedColor_ = {1.0f,1.0f,1.0f,1.0f}; // SINGLEまたはSIMILAR用の基準色
    float colorVariation_ = 0.1f; // 類似色モードでのバラつき度合い(0.0f〜1.0f程度)


protected:
    //===================================================================*/
    //                    protected methods
    //===================================================================*/
    std::unique_ptr<ParticleBehavior> behavior_;


public:
    //===================================================================*/
    //                    getter/setter
    //===================================================================*/

    const std::string& GetName() const{ return name_; }
    void SetName(const std::string& name){ name_ = name; }
    void SetEmitPos(const Vector3& pos){ emitter_.transform.translate = pos; }
	void SetEmitRotate(const Vector3& rotate){ emitter_.transform.rotate = rotate; }
    // BaseParticleの仮想関数をオーバーライド
    bool GetUseRandomColor() const override;
    Vector4 GetSelectedColor() const override;

    const Vector3& GetEmitterPos()const{ return emitter_.transform.translate; }

};