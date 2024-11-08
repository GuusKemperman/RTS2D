#include "precomp.h"
#include "Opponent.h"

#include "Army.h"
#include "Unit.h"
#include "Scene.h"
#include "EntityManager.h"
#include "Group.h"
#include "Quadtree.h"
#include "TimeManager.h"

Opponent::Opponent(Scene& scene, Army& army) :
	Entity(scene),
	myArmy(army)
{

}

Opponent::~Opponent()
{
}

void Opponent::Tick()
{
	float deltaTime = TimeManager::GetDeltaTime();

	timeSinceLastTick += deltaTime;

	if (timeSinceLastTick < tickTimestep)
	{
		return;
	}
	else
	{
		timeSinceLastTick -= tickTimestep;
	}


	uint groupDesiredSize = RandomGroupSize();

	vector<Group*> groups = myArmy.GetGroupsInArmy();
	vector<Group*> idleGroups{};
	idleGroups.reserve(groups.size());

	float nonIdleChance = 1.0f - powf(1.0f - nonIdleGroupOrderChance, deltaTime);

	for (Group* group : groups)
	{
		vector<float> distribution = group->GetStatesDistribution();

		float idle = distribution[static_cast<size_t>(Unit::State::idle)];
		float moving = distribution[static_cast<size_t>(Unit::State::movement)];

		if (idle == 1.0f ||
			(idle + moving) * nonIdleChance > Rand(1.0f))
		{
			idleGroups.push_back(group);
			continue;
		}

		vector<Unit*> unitsInGroup = group->GetUnitsInGroup();
	}

	for (size_t i = 0; i < idleGroups.size(); i++)
	{
		Group* mainGroup = idleGroups[i];
		mainGroup->RemoveInvalidIds();
		uint desiredGroupSize = RandomGroupSize();

		for (size_t j = i + 1; j < idleGroups.size() && mainGroup->unitsInGroupIds.size() < desiredGroupSize; j++, i++)
		{
			idleGroups[j]->RemoveInvalidIds();
			*mainGroup += *idleGroups[j];
		}

		OrderToRandomPosition(*mainGroup);
	}
}

uint Opponent::RandomGroupSize() const
{
	return RandomUInt() % (maxGroupSize - minGroupSize) + minGroupSize;
}

void Opponent::OrderToRandomPosition(Group& group) const
{
	const BoundingBox& maxBounds = scene.quadTree->GetMaxBounds();
	float2 position = float2{ Rand(maxBounds.GetRight() - 2.0f * minDistFromEdge), Rand(maxBounds.GetBottom() - 2.0f * minDistFromEdge) } + float2{ minDistFromEdge };
	MoveTo command{ position, AggroLevel::aggressive, optional<float>() };
	Group::Formation formation = group.CalculateFormation(command);
	group.FormFormation(formation);
}

