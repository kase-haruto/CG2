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

public:
    //===================================================================*/
    //                    getter/setter
    //===================================================================*/

    const std::string& GetName() const{ return name_; }
    void SetName(const std::string& name){ name_ = name; }
    void SetEmitPos(const Vector3& pos){ emitter_.transform.translate = pos; }

private:
    //===================================================================*/
    //                    private methods
    //===================================================================*/

    std::string name_;                  // システム名

protected:
    //===================================================================*/
    //                    protected methods
    //===================================================================*/


};