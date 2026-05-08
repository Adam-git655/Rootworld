#include "UISystem.h"

void UISystem::renderUI(EntityManager& mgr, Entt playerEntity, sf::Texture& heartTexture, sf::RenderWindow& window)
{
	renderHearts(mgr, playerEntity, heartTexture, window);
}

void UISystem::renderHearts(EntityManager& mgr, Entt playerEntity, sf::Texture& heartTexture, sf::RenderWindow& window) const
{
	auto& health = mgr.getComponent<HealthComponent>(playerEntity);
	int totalHearts = health.maxHealth / 10;
	float playerHp = health.health;

	sf::Sprite heartSprite(heartTexture);
	heartSprite.setScale(2, 2);

	for (int i = 0; i < 10; ++i)
	{
		float currentHeartHealth = std::clamp(playerHp - (i * 10.0f), 0.0f, 10.0f);
		float healthRatioInHeart = currentHeartHealth / 10.0f;

		int frame;
		if (healthRatioInHeart > 0.75f)
			frame = 0;
		else if (healthRatioInHeart > 0.5f)
			frame = 1;
		else if (healthRatioInHeart > 0.25f)
			frame = 2;
		else if (healthRatioInHeart > 0.0f)
			frame = 3;
		else
			frame = 4;

		heartSprite.setTextureRect({ frame * heartWidth, 0, heartWidth, heartHeight });
		heartSprite.setPosition(healthBarPosX + i * ((heartWidth * heartSprite.getScale().x) + 4.0f), healthBarPosY);
		window.draw(heartSprite);
	}

}