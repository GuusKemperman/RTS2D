#pragma once
#include "enums.h"
#include "Scene.h"
#include "EntityManager.h"

class Group;

class Army
{
public:
	Army(Scene& scene, ArmyId id);
	
	const vector<EntityId>& GetIds();
	vector<Group*> GetGroupsInArmy();

	template<typename T, typename ...Args>
	inline T& AddToArmy(Args && ...args);

	const ArmyId armyId{ ArmyId::null };

	Scene& scene;
private:
	void RemoveInvalidIds();

	// List of entities in army
	vector<EntityId> militaryIds{};
};

template<typename T, typename ...Args>
inline T& Army::AddToArmy(Args && ...args)
{
	T& newEntity = scene.entityManager->AddEntity<T>(*this, forward<Args>(args)...);
	militaryIds.push_back(newEntity.GetId());
	return newEntity;
}