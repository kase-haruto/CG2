#pragma once

#include "BaseParticle.h"

#include <string>
#include "particleBehavior/ParticleBehavior.h"

class Particle 
    : public BaseParticle{
public:
    //===================================================================*/
    //                    public methods
    //===================================================================*/

    Particle();
    ~Particle() override;

    virtual void Initialize(const std::string& modelName, const std::string& texturePath);

    /* ui ===============================================*/
    virtual void ImGui();
    void Load(const std::string& filename);
    void Save(const std::string& filename);

protected:
    //===================================================================*/
    //                    private methods
    //===================================================================*/
    std::string fileDirectoryPath = "./Resources/json/particle/";

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