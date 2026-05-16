#pragma once
#include "SFML/Graphics.hpp"

struct LightEmitterComponent
{
	int lightLevel = 5;
	sf::Color lightColor = sf::Color::White;
	bool canUpdate = true;
	int lastTileX = -1;
	int lastTileY = -1;
};
