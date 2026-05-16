#include "LightEmitterSystem.h"

void LightEmitterSystem::update(EntityManager& mgr, LightingSystem& lightingSystem, ChunksManager& chunksManager)
{
	auto& lightEmitterStorage = mgr.getComponentStorage<LightEmitterComponent>();
	auto& transformStorage = mgr.getComponentStorage<TransformComponent>();

	for (auto& [e, lightEmitter] : lightEmitterStorage.getAll())
	{
		if (!lightEmitter.canUpdate)
			continue;

		auto& transform = transformStorage.get(e);

		int tileX = static_cast<int>(transform.position.x) / Chunk::TILESIZE;
		int tileY = static_cast<int>(transform.position.y) / Chunk::TILESIZE;

		if (lightEmitter.lastTileX != -1)
			lightingSystem.RemoveLightSource(lightEmitter.lastTileX, lightEmitter.lastTileY);

		lightingSystem.AddLightSource(tileX, tileY, lightEmitter.lightLevel, lightEmitter.lightColor);
		chunksManager.UpdateLightingForRegion(tileX, tileY);

		lightEmitter.lastTileX = tileX;
		lightEmitter.lastTileY = tileY;
		lightEmitter.canUpdate = false;
	}
}