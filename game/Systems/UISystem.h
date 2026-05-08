#pragma once
#include "Item.h"
#include "engine/ecs/EntityManager.h"
#include "game/Components/HealthComponent.h"
#include "game/Components/InventoryComponent.h"
#include "engine/world/ChunksManager.h"

#include "Systems/RenderSystem.h"
#include "game/SoundManager.h"

#include "SFML/Graphics.hpp"
#include "imgui.h"
#include "imgui-SFML.h"

#include <iostream>

class UISystem
{
public:
	void renderUI(EntityManager& mgr, Entt playerEntity,
		sf::Texture& heartTexture, bool renderInv, 
		std::unordered_map<std::string, ItemDef>& itemRegistry, ChunksManager& chunksManager, sf::Texture& shortSwordTex,
		RenderSystem& renderSystem, SoundManager& soundManager, bool& isLighting, bool& showAIDebugLines, float& masterVolume, 
		sf::RenderWindow& window);

private:
	const int heartWidth = 17;
	const int heartHeight = 17;
	
	const float healthBarPosX = 1200.0f;
	const float healthBarPosY = 10.0f;

	void renderHearts(EntityManager& mgr, Entt playerEntity, sf::Texture& heartTexture, sf::RenderWindow& window) const;
	void renderHotbar(EntityManager& mgr, Entt playerEntity, std::unordered_map<std::string, ItemDef>& itemRegistry, ChunksManager& chunksManager, sf::Texture& shortSwordTex);
	void renderInventory(EntityManager& mgr, Entt playerEntity, bool renderInv, std::unordered_map<std::string, ItemDef>& itemRegistry, ChunksManager& chunksManager, sf::Texture& shortSwordTex);
	void renderSettings(ChunksManager& chunksManager, RenderSystem& renderSystem, SoundManager& soundManager, bool& isLighting, bool& showAIDebugLines, float& masterVolume);
};