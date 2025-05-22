#include "TransformSnapshot.h"

TransformSnapshot TransformSnapshot::Capture(const SceneObject* obj) {
    const auto& tf = obj->GetWorldTransform();
    return { tf.translation, tf.rotation, tf.scale };
}

void TransformSnapshot::Apply(SceneObject* obj) const {
    auto& tf = obj->GetWorldTransform();
    tf.translation = translate;
    tf.rotation = rotate;
    tf.scale = scale;
}
