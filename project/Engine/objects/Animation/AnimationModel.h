#pragma once
#include "../Model.h"

class AnimationModel :
    public Model{

public:
	//===================================================================*/
	//                  public  methods
	//===================================================================*/

	AnimationModel(const std::string& fileName);
	AnimationModel() = default;
	~AnimationModel() = default;

	void Update();

private:
    //===================================================================*/
    //                  private  methods
    //===================================================================*/
	float animationTime_ = 0.0f;	//< アニメーションの時間

};

