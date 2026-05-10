#pragma once

//World Clock
struct WorldClock
{
	float timeOfDay = 22.0f;
	float daySpeed = 0.1f;
	int day = 1;

	float dayProgress() const
	{
		return timeOfDay / 24.0f;
	}

	bool isNight() const
	{
		return timeOfDay < 6.0f || timeOfDay >= 20.0f;
	}
};