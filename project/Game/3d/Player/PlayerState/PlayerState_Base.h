#pragma once

class Player;

class PlayerState_Base{
public:
	//===================================================================*/
	//                    main functions
	//===================================================================*/
	PlayerState_Base() = default;
	PlayerState_Base(Player* player);
	virtual ~PlayerState_Base() = default;
	
	virtual void Initialize() = 0;
	virtual void Update() = 0;
	virtual void Draw() = 0;
	virtual void Cleanup() = 0;

	virtual void ShowGui() = 0;

protected:
	//===================================================================*/
	//                    protected variables
	//===================================================================*/
	Player* pPlayer_ = nullptr;
};

