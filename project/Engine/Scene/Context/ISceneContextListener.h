#pragma once
class SceneContext;

class ISceneContextListener {
public:
	virtual ~ISceneContextListener() = default;
	virtual void OnSceneContextChanged(SceneContext* newContext) = 0;
};