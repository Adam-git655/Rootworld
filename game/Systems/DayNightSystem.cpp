#include "DayNightSystem.h"

void DayNightSystem::update(EntityManager& mgr, WorldClock& worldClock, float dt)
{
	worldClock.timeOfDay += dt * worldClock.daySpeed;

	if (worldClock.timeOfDay >= 24.0f)
	{
		worldClock.timeOfDay = 0.0f;
		worldClock.day += 1;
	}

	//filter enemies based on wether they can spawn during day
	auto& enemyDayNightStorage = mgr.getComponentStorage<EnemyDayNightComponent>();
	for (auto& [e, enemyDayNight] : enemyDayNightStorage.getAll())
	{
		if (enemyDayNight.isNightOnlyEnemy && !worldClock.isNight())
		{
			mgr.destroy(e);
		}
	}
}