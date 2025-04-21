#include "SceneContext.h"
#include <lib/myFunc/PrimitiveDrawer.h>

SceneContext::SceneContext(){
	meshRenderer_ = std::make_unique<MeshRenderer>();
}
