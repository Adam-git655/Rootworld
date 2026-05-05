#pragma once
#include "game/WorldClock.h"

class DayNightSystem
{
public:
	void update(WorldClock& worldClock, float dt);
};