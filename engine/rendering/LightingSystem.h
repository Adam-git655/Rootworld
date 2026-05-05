#pragma once
#include "engine/world/Chunk.h"
#include <queue>
#include <unordered_map>
#include <vector>
#include <SFML/Graphics.hpp>

struct LightNode { int worldX, y, level; };

struct LightSource {
	int worldX, worldY, lightLevel;
	sf::Color color;
};

class LightingSystem
{
public:
	LightingSystem();

	void UpdateLighting(const std::unordered_map<int, Chunk*>& renderedChunks);
	void UpdateLightingRegion(const std::unordered_map<int, Chunk*>& renderedChunks, int worldX, int worldY);

	void AddLightSource(int worldX, int worldY, int lightLevel, sf::Color color);
	void RemoveLightSource(int worldX, int worldY);
	void UpdateLightSources(const std::unordered_map<int, Chunk*>& renderedChunks);

	void setLightingForTimeOfDay(float timeProgress);

	sf::Color getSkyColor(float timeProgress);
	sf::Color InterPolateSkyColorOnLightLevel(int chunkX, int localX, int y, float timeProgress);

private:
	std::unordered_map<int, std::vector<std::vector<sf::Color>>> lightMaps;
	std::unordered_map<int, std::vector<std::vector<int>>> lightLevelMaps;
	std::vector<LightSource> lightSources;

	const sf::Color NOON_COLOR = sf::Color(0, 191, 255);
	const sf::Color MIDNIGHT_COLOR = sf::Color(20, 24, 82);
	const sf::Color WHITE_LIGHT = sf::Color::White;
	const sf::Color AMBIENT_LIGHT = sf::Color::Black;
	const float MIN_MAX_LIGHT_LEVEL = 4.0f;
	const float MAX_MAX_LIGHT_LEVEL = 15.0f;
	const int PLAYER_LIGHT_RADIUS = 12;

	float MAX_LIGHT_LEVEL = MAX_MAX_LIGHT_LEVEL;

	void PropogateSunlight(const std::unordered_map<int, Chunk*>& renderedChunks);
	void RunBFS(const std::unordered_map<int, Chunk*>& renderedChunks, std::queue<LightNode> nodesToProcess);
	sf::Color InterpolateColor(float t) const;
};
