#include "precomp.h"
#include "Military.h"
#include "Army.h"
#include "EntityManager.h"
#include "Scene.h"
#include "Unit.h"
#include "Group.h"

Army::Army(Scene& a_scene, ArmyId id) :
	scene(a_scene),
	armyId(id)
{
	
}

const vector<EntityId>& Army::GetIds()
{
	RemoveInvalidIds();
	return militaryIds;
}

vector<Group*> Army::GetGroupsInArmy()
{
	EntityManager* entiyManager = scene.entityManager.get();
	RemoveInvalidIds();
	vector<EntityId> idsGroups{};
	vector<Group*> groups{};

	for (EntityId id : militaryIds)
	{
		Unit* unit = dynamic_cast<Unit*>(entiyManager->TryGetEntity(id).value_or(nullptr));

		if (unit == nullptr
			|| find(idsGroups.begin(), idsGroups.end(), unit->partOfGroup) != idsGroups.end())
		{
			continue;
		}

		idsGroups.push_back(unit->partOfGroup);
		Group* group = dynamic_cast<Group*>(entiyManager->TryGetEntity(unit->partOfGroup).value_or(nullptr));

		if (group != nullptr)
		{
			groups.push_back(group);
		}
	}

	return groups;
}

void Army::RemoveInvalidIds()
{
	EntityManager* entityManager = scene.entityManager.get();
	militaryIds.erase(remove_if(militaryIds.begin(), militaryIds.end(), [entityManager](const EntityId& id)
		{
			return !entityManager->TryGetEntity(id).has_value();
		}), militaryIds.end());
}
