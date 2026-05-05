#pragma once

//World Clock
struct WorldClock
{
	float timeOfDay = 6.0f;
	float daySpeed = 0.2f;
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