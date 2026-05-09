#pragma once
#include "engine/core/Vec2.h"
#include "engine/world/ChunksManager.h"
#include "game/EntityFactory.h"

enum class Enemies
{
	Zombie,
	BloodBat,
	None
};

class EnemySpawnerSystem
{
public:
	void update(EntityManager& mgr, EntityFactory& entityFactory, const std::unordered_map<Enemies, sf::Texture*>& enemyTextures, ChunksManager& chunksManager, Entt playerEntity, float dt);

	void setEnemySpawnChance(Enemies enemy, float newChance);
	float defaultZombieSpawnChance = 0.6f;
	float defaultBatSpawnChance = 0.4f;

private:
	std::mt19937 gen{ std::random_device{}() };

	float spawnTimer = 0.0f;
	float spawnInterval = 4.0f;
	float spawnRangeMin = 200.0f;
	float spawnRangeMax = 1000.0f;

	std::vector<std::pair<Enemies, float>> enemySpawnChancesMap =
	{
		{Enemies::Zombie, defaultZombieSpawnChance},
		{Enemies::BloodBat, defaultBatSpawnChance}
	};

	Enemies pickRandomEnemy();
	Vec2 pickSpawnPosZombie(Vec2 playerPos, ChunksManager& chunksManager);
	Vec2 pickSpawnPosBloodBat(Vec2 playerPos);
};