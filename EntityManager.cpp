#include "precomp.h"
#include "EntityManager.h"
#include "Entity.h"
#include "Scene.h"

EntityManager::EntityManager(Scene& a_scene) :
	scene(a_scene)
{

}

EntityManager::~EntityManager()
{
	entityLookUp.clear();
	entities.clear();
}

void EntityManager::TickEntities()
{
	while (!toRemove.empty())
	{
		EntityId id = toRemove.back();
		toRemove.pop_back();

		auto it = entityLookUp.find(id);
		Entity* entity = it->second;
		entityLookUp.erase(it);

		auto itToErase = find_if(entities.begin(), entities.end(),
			[entity](const unique_ptr<Entity>& a)
			{
				return a.get() == entity;
			});
		size_t index = itToErase - entities.begin();

		entities[index] = move(entities.back());
		entities.pop_back();
	}

	for (size_t i = 0; i < entities.size(); i++)
	{
		entities[i]->Tick();
	}
}

void EntityManager::RemoveEntity(EntityId id)
{
	if (auto it = find(toRemove.begin(), toRemove.end(), id); it != toRemove.end())
	{
		LOGWARNING("Removing entity " << id << " twice");
	}
	else
	{
		toRemove.push_back(id);
	}
}

Entity* EntityManager::GetEntity(EntityId id)
{
	return entityLookUp.at(id);
}

optional<Entity*> EntityManager::TryGetEntity(EntityId id)
{
	auto it = entityLookUp.find(id);

	if (it != entityLookUp.end())
	{
		return optional<Entity*>(it->second);
	}

	static optional<Entity*> emptyOptional{};
	return emptyOptional;
}

void EntityManager::RemoveInvalidIds(vector<EntityId>& ids)
{
	const unordered_map<EntityId, Entity*>& lookUp = entityLookUp;
	ids.erase(std::remove_if(ids.begin(), ids.end(),
		[lookUp](const EntityId& id) -> bool
		{
			return lookUp.find(id) == lookUp.end();
		}), ids.end());
}
