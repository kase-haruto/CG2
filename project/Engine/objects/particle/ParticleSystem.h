#pragma once

#include "BaseParticle.h"

#include <string>

class ParticleSystem 
    : public BaseParticle{
public:
    //===================================================================*/
    //                    public methods
    //===================================================================*/

    ParticleSystem();
    ~ParticleSystem() override = default;

    virtual void Initialize(const std::string& modelName, const std::string& texturePath);


    virtual void ImGui();

public:
    //===================================================================*/
    //                    getter/setter
    //===================================================================*/

    const std::string& GetName() const{ return name_; }
    void SetName(const std::string& name){ name_ = name; }
    void SetEmitPos(const Vector3& pos){ emitter_.transform.translate = pos; }

    // BaseParticleの仮想関数をオーバーライド
    bool GetUseRandomColor() const override{ return useRandomColor_; }
    Vector4 GetSelectedColor() const override{ return selectedColor_; }

private:
    //===================================================================*/
    //                    private methods
    //===================================================================*/
    std::string name_;                                  // システム名
    bool useRandomColor_ = true;                        // ランダムカラーを使用するか
    Vector4 selectedColor_ = {1.0f, 1.0f, 1.0f, 1.0f};  // ランダムでない場合に使う色

protected:
    //===================================================================*/
    //                    protected methods
    //===================================================================*/


};