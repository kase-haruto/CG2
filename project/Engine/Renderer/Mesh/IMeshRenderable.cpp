#include "IMeshRenderable.h"

IMeshRenderable::~IMeshRenderable() {
		// 死ぬときに全リスナーに通知！
	for (auto* listener : deathListeners_) {
		listener->OnRenderableDestroyed(this);
	}
}

void IMeshRenderable::RemoveDeathListener(IRenderableDeathListener* listener) {
	deathListeners_.erase(
		std::remove(deathListeners_.begin(), deathListeners_.end(), listener),
		deathListeners_.end()
	);
}
