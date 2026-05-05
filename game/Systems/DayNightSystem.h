#pragma once
#include "game/WorldClock.h"
#include "engine/ecs/EntityManager.h"
#include "game/Components/EnemyDayNightComponent.h"

class DayNightSystem
{
public:
	void update(EntityManager& mgr, WorldClock& worldClock, float dt);
};