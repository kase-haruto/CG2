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

    // システムの初期化
    virtual void Initialize(const std::string& modelName, const std::string& textureFilePath, uint32_t count);

public:
    //===================================================================*/
    //                    getter/setter
    //===================================================================*/

    const std::string& GetName() const{ return name_; }
    void SetName(const std::string& name){ name_ = name; }

protected:
    //===================================================================*/
    //                    protected methods
    //===================================================================*/
    ParticleData::Emitter emitter_; // エミッター
    std::string name_;              // システム名
};