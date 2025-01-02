#include "PlayerState_Stay.h"

#include <externals/imgui/imgui.h>

PlayerState_Stay::PlayerState_Stay(Player* player)
	:PlayerState_Base(player){}

//*===================================================================*/
//                    main Func	*/
/*===================================================================*/
void PlayerState_Stay::Initialize(){}

void PlayerState_Stay::Update(){}

void PlayerState_Stay::Draw(){}

void PlayerState_Stay::Cleanup(){}

//*===================================================================*/
//                    ui	*/
/*===================================================================*/
void PlayerState_Stay::ShowGui(){

	ImGui::Text("PlayerState_Stay");

	ImGui::Separator();

	ImGui::DragFloat("animationSpeed", &animationSpeed_, 0.01f);
}
