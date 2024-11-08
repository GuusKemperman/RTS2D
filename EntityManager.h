#pragma once
#include "enums.h"

class Entity;
class Scene;

class EntityManager
{
public:
	EntityManager(Scene& scene);
	~EntityManager();

	EntityManager(const EntityManager&) = delete;
	EntityManager& operator=(const EntityManager&) = delete;

	void TickEntities();

	template<typename T, typename ...Args>
	inline T& AddEntity(Args && ...args);
	void RemoveEntity(EntityId id);

	Entity* GetEntity(EntityId id);
	optional<Entity*> TryGetEntity(EntityId id);

	void RemoveInvalidIds(vector<EntityId>& ids);

private:
	Scene& scene;

	vector<unique_ptr<Entity>> entities{};
	unordered_map<EntityId, Entity*> entityLookUp{};

	vector<EntityId> toRemove;
};

template<typename T, typename ...Args>
inline T& EntityManager::AddEntity(Args && ...args)
{
	unique_ptr<T> newEntity = make_unique<T>(scene, forward<Args>(args)...);
	entityLookUp[newEntity->GetId()] = newEntity.get();

	T* toReturn = newEntity.get();
	entities.push_back(move(newEntity));

	return *toReturn;
}