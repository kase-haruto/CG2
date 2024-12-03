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

    virtual void Initialize(const std::string& modelName);

public:
    //===================================================================*/
    //                    getter/setter
    //===================================================================*/

    const std::string& GetName() const{ return name_; }
    void SetName(const std::string& name){ name_ = name; }

private:
    //===================================================================*/
    //                    private methods
    //===================================================================*/

    std::string name_;                  // システム名

protected:
    //===================================================================*/
    //                    protected methods
    //===================================================================*/
    ParticleData::Emitter emitter_;     // エミッター

};