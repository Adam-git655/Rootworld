#pragma once
#include <unordered_map>
#include <memory>
#include <typeindex>

#include "Entity.h"
#include "ComponentStorage.h"

class EntityManager
{
public:
	Entity create()
	{
		return entityID++;
	}

	template<typename T>
	void addComponent(Entity e, const T& component)
	{
		getStorage<T>().add(e, component);
	}

	template<typename T>
	void removeComponent(Entity e)
	{
		getStorage<T>().remove(e);
	}

	template<typename T>
	bool hasComponent(Entity e)
	{
		return getStorage<T>().has(e);
	}

	template<typename T>
	T& getComponent(Entity e)
	{
		return getStorage<T>().get(e);
	}

	template<typename T>
	ComponentStorage<T>& getComponentStorage()
	{
		return getStorage<T>();
	}

	void destroy(Entity e)
	{
		for (auto& [type, storage] : storages)
		{
			storage->remove(e);
		}
	}

private:
	Entity entityID = 0;

	std::unordered_map<std::type_index, std::shared_ptr<IComponentStorage>> storages;

	template<typename T>
	ComponentStorage<T>& getStorage()
	{
		std::type_index index = typeid(T);

		if (storages.find(index) == storages.end())
		{
			storages[index] = std::make_shared<ComponentStorage<T>>();
		}

		return *static_cast<ComponentStorage<T>*>(storages[index].get());
	}

};