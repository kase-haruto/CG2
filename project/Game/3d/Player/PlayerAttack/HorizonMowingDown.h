#pragma once
#include "IPlayerAttack.h"

class HorizonMowingDown :
    public IPlayerAttack{
public:
	//===================================================================*/
	//                   public member functions
	//===================================================================*/
	HorizonMowingDown(const std::string& attackName);
	~HorizonMowingDown() override = default;

	/* main ===================================*/
	void Initialize()override;	//< 初期化
	void Execution()override;	//< 実行
	void Update()override;		//< 更新
	void Draw()override;		//< 描画

	/* gui ui ===================================*/
	void ShowGui()override;		//< gui表示

	/* collision ===================================*/
	void OnCollisionEnter(Collider* other)override;
	void OnCollisionStay(Collider* other)override;
	void OnCollisionExit(Collider* other)override;

private:
	//===================================================================*/
	//                   private member functions
	//===================================================================*/
};

