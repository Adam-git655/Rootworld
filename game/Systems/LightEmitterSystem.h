#pragma once
#include "game/Components/LightEmmiterComponent.h"
#include "engine/ecs/EntityManager.h"
#include "engine/rendering/LightingSystem.h"
#include "engine/world/ChunksManager.h"

class LightEmitterSystem
{
public:
	void update(EntityManager& mgr, LightingSystem& lightingSystem, ChunksManager& chunksManager);
};