#include "precomp.h"
#include "Group.h"
#include "Military.h"
#include "EntityManager.h"
#include "ColliderComponent.h"
#include "TransformComponent.h"
#include "Unit.h"
#include "TimeManager.h"
#include "SelectingComponent.h"
#include "Scene.h"

Group::Group(Scene& scene, const vector<size_t>& ids, bool ownsUnits) :
	Entity(scene),
	unitsInGroupIds(ids)
{
	if (ownsUnits)
	{
		for (EntityId id : unitsInGroupIds)
		{
			Unit* unit = dynamic_cast<Unit*>(scene.entityManager->TryGetEntity(id).value_or(nullptr));

			if (unit != nullptr)
			{
				unit->partOfGroup = GetId();
			}
		}

		RemoveInvalidIds();
	}
}

Group::~Group()
{

}

void Group::operator+=(Group& other)
{
	vector<Unit*> otherUnits = other.GetUnitsInGroup();

	for (Unit* unit : otherUnits)
	{
		unitsInGroupIds.push_back(unit->GetId());
		unit->partOfGroup = GetId();
	}

	other.unitsInGroupIds.clear();
}

void Group::Tick()
{
	float currentTime = TimeManager::GetTotalTimePassed();
	float timeSinceLastUpdate = currentTime - timeLastUpdate;
	if (timeSinceLastUpdate < updateCooldown)
	{
		return;
	}

	RemoveInvalidIds();
	if (unitsInGroupIds.empty())
	{
		scene.entityManager->RemoveEntity(GetId());
		return;
	}

	if (desiredFormation.has_value())
	{
		AssignUnitsToFormation(GetUnitsInGroup(), desiredFormation.value());
		FormFormation(desiredFormation.value());
	}

	timeLastUpdate = currentTime;
}


float2 Group::GetGroupCentre(const vector<Unit*>& units) const
{
	float2 groupCentre{};
	
	for (Unit* unit : units)
	{
		groupCentre += unit->GetComponent<TransformComponent>()->GetWorldPos();
	}
	return groupCentre / static_cast<float>(units.size());
}

void Group::RemoveInvalidIds()
{
	EntityManager* entityManager = scene.entityManager.get();

	EntityId groupId = GetId();
	unitsInGroupIds.erase(std::remove_if(unitsInGroupIds.begin(), unitsInGroupIds.end(),
		[groupId, entityManager](const EntityId& id) -> bool
		{
			Unit* unit = dynamic_cast<Unit*>(entityManager->TryGetEntity(id).value_or(nullptr));
			return unit == nullptr || unit->partOfGroup != groupId;
		}), unitsInGroupIds.end());
}

vector<float> Group::GetStatesDistribution() const
{
	vector<float> distribution(static_cast<size_t>(Unit::State::numOfStates), 0.0f);
	vector<Unit*> unitsInGroup = GetUnitsInGroup();

	for (Unit* unit : unitsInGroup)
	{
		distribution[static_cast<size_t>(unit->GetState())] += 1.0f;
	}

	for (float& value : distribution)
	{
		value /= unitsInGroup.size();
	}

	return distribution;
}

Group::Formation Group::CalculateFormation(const MoveTo& moveTo) const
{
	vector<Unit*> unitsInGroup = GetUnitsInGroup();

	if (unitsInGroup.empty())
	{
		return Formation{moveTo};
	}

	// Generate points
	Unit* biggestUnit = *max_element(unitsInGroup.begin(), unitsInGroup.end(),
		[](const Unit* a, const Unit* b)
		{
			return a->GetComponent<ColliderComponent>()->GetRadius() < b->GetComponent<ColliderComponent>()->GetRadius();
		});

	float spacing = biggestUnit->GetComponent<ColliderComponent>()->GetRadius() * 2.0f + biggestUnit->desiredSpacing * 1.0f;

	const int groupSize = static_cast<int>(unitsInGroup.size());

	Formation formation{moveTo};
	float2 deltaPosGroup = moveTo.destination - GetGroupCentre(unitsInGroup);

	formation.rotation = moveTo.rotationAtEnd.value_or(Float2ToAngle(deltaPosGroup));
	formation.points.resize((groupSize * 2) * (groupSize * 2));

	// Generating all the points can be optimized; Im creating and sorting points that are discarded. 
	// this would be faster if points were generated from the centre. 
	size_t pointIndex = 0;
	for (int x = -groupSize; x < groupSize; x++)
	{
		for (int y = -groupSize; y < groupSize; y++, pointIndex++)
		{
			FormationPoint& point = formation.points[pointIndex];
			point.position = (make_float2(static_cast<float>(x), static_cast<float>(y)) * spacing + moveTo.destination);
			point.sqrDistToCentre = SqrdDistance(point.position, moveTo.destination);
		}
	}

	// Sort the points by distance to the centre
	sort(formation.points.begin(), formation.points.end(),
		[](const Group::FormationPoint& a, const Group::FormationPoint& b) -> bool
		{
			return a.sqrDistToCentre < b.sqrDistToCentre;
		});

	formation.points.resize(groupSize);

	AssignUnitsToFormation(unitsInGroup, formation);

	return formation;
}

void Group::FormFormation(const Formation& formation)
{
	desiredFormation = formation;
	
	EntityManager& entityManager = *scene.entityManager;
	for (const FormationPoint& fp : formation.points)
	{
		auto idIt = find(unitsInGroupIds.begin(), unitsInGroupIds.end(), fp.assignedUnit);

		if (idIt == unitsInGroupIds.end())
		{
			// Unit has left the group or has been destroyed in the meantime
			continue;
		}

		optional<Entity*> entity = entityManager.TryGetEntity(*idIt);

		if (!entity.has_value())
		{
			continue;
		}

		Unit* unit = dynamic_cast<Unit*>(entity.value());
		assert(unit != nullptr);

		unit->CommandMoveTo(MoveTo{ fp.position, formation.givenCommand.aggroLevel, formation.rotation });
	}
}

vector<Unit*> Group::GetUnitsInGroup() const
{
	vector<Unit*> units;
	units.reserve(unitsInGroupIds.size());
	EntityManager& entityManager = *scene.entityManager;

	for (size_t i = 0; i < unitsInGroupIds.size(); i++)
	{
		Unit* unit = dynamic_cast<Unit*>(entityManager.TryGetEntity(unitsInGroupIds[i]).value_or(nullptr));
		
		if (unit != nullptr)
		{
			units.push_back(unit);
		}

	}
	
	return units;
}

struct UnitToMove
{
	Unit* unit{};
	float2 positionAfterGoupMovement{};
};

void Group::AssignUnitsToFormation(const vector<Unit*>& units, Formation& formation) const
{
	float2 deltaPosGroup = formation.givenCommand.destination - GetGroupCentre(units);

	vector<UnitToMove> unitsToMove(units.size());
	for (size_t i = 0; i < units.size(); i++)
	{
		Unit* unit = units[i];

		float2 position = unit->GetComponent<TransformComponent>()->GetWorldPos();
		float2 positionAfterMove = position + deltaPosGroup;
		unitsToMove[i] = { unit, positionAfterMove };
	}

	// Units left the group, remove the furthest points.
	formation.points.resize(unitsToMove.size());

	for (Group::FormationPoint& fp : formation.points)
	{
		auto closest = min_element(unitsToMove.begin(), unitsToMove.end(),
			[fp](const UnitToMove& a, const UnitToMove& b)
			{
				return SqrdDistance(a.positionAfterGoupMovement, fp.position) < SqrdDistance(b.positionAfterGoupMovement, fp.position);
			});

		fp.assignedUnit = closest->unit->GetId();
		unitsToMove.erase(closest);
	}
}

