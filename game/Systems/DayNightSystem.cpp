#include "DayNightSystem.h"

void DayNightSystem::update(WorldClock& worldClock, float dt)
{
	worldClock.timeOfDay += dt * worldClock.daySpeed;

	if (worldClock.timeOfDay >= 24.0f)
	{
		worldClock.timeOfDay = 0.0f;
		worldClock.day += 1;
	}
}