#pragma once
#include "engine/ecs/EntityManager.h"
#include "SFML/Graphics.hpp"

#include "game/Components/HealthComponent.h"
#include <iostream>

class UISystem
{
public:
	void renderUI(EntityManager& mgr, Entt playerEntity, sf::Texture& heartTexture, sf::RenderWindow& window);

private:
	const int heartWidth = 17;
	const int heartHeight = 17;
	
	const float healthBarPosX = 1200.0f;
	const float healthBarPosY = 10.0f;

	void renderHearts(EntityManager& mgr, Entt playerEntity, sf::Texture& heartTexture, sf::RenderWindow& window) const;
};