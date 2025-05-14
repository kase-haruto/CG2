#include "AABB.h"


/* external */
#include "externals/imgui/imgui.h"
/* c++ */
#include <cmath>


void AABB::Initialize(const Vector3& Min, const Vector3& Max){
    min = Min;
    max = Max;
}

void AABB::Update(){
    //minとmaxが入れ替わらないようにする
    min.x = ( std::min ) (min.x, max.x);
    max.x = ( std::max ) (min.x, max.x);

    min.y = ( std::min ) (min.y, max.y);
    max.y = ( std::max ) (min.y, max.y);

    min.z = ( std::min ) (min.z, max.z);
    max.z = ( std::max ) (min.z, max.z);
}



void AABB::UpdateUI(std::string lavel){
    ImGui::Begin(lavel.c_str());
    ImGui::DragFloat3("min", &min.x, 0.01f);
    ImGui::DragFloat3("max", &max.x, 0.01f);
    ImGui::End();
}

Vector3 AABB::GetMin()const{ return min; }
Vector3 AABB::GetMax()const{ return max; }

