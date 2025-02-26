#pragma once
#include "BaseEditor.h"

//forward declaration
class ParticleManager;

class ParticleEditor :
    public BaseEditor{
public:
    //===================================================================*/
    //                    public functions
    //===================================================================*/
	ParticleEditor();
	~ParticleEditor() = default;

	void ShowImGuiInterface() override;

private:
	//===================================================================*/
	//                    private functions
	//===================================================================*/
	void AddParticle4Manager();

private:
	//===================================================================*/
	//                   　private variables
	//===================================================================*/
	ParticleManager* pParticleManager_;

public:
	//===================================================================*/
	//                    getter / setter
	//===================================================================*/
	void SetParticleManager(ParticleManager* pParticleManager);
};