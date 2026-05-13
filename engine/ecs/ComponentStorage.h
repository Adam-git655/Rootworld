#pragma once
#include "engine/ecs/Entity.h"
#include <unordered_map>
#include <vector>

class IComponentStorage
{
public:
	virtual ~IComponentStorage() = default;
	virtual void remove(Entity e) = 0;
};

template<typename T>
class ComponentStorage : public IComponentStorage
{
public:
	void add(Entity e, const T& component)
	{
		components[e] = component;
	}

	void remove(Entity e) override
	{
		components.erase(e);
	}

	bool has(Entity e)
	{
		return components.find(e) != components.end();
	}

	T& get(Entity e)
	{
		return components[e];
	}

	std::unordered_map<Entity, T>& getAll()
	{
		return components;
	}

private:
	std::unordered_map<Entity, T> components;
};