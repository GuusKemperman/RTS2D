#pragma once
#include "Entity.h"
#include "Commands.h"

class Unit;

class Group : 
	public Entity
{
public:
	Group(Scene& scene, const vector<size_t>& ids, bool ownsUnits);
	~Group();

	void operator+=(Group& other);

	void Tick();

	float2 GetGroupCentre(const vector<Unit*>& units) const;

	void RemoveInvalidIds();

	vector<float> GetStatesDistribution() const;

	struct FormationPoint
	{
		float2 position{};
		float sqrDistToCentre{};
		size_t assignedUnit{};
	};

	struct Formation
	{
		MoveTo givenCommand{};
		vector<FormationPoint> points{};
		float rotation{};
	};

	Formation CalculateFormation(const MoveTo& moveTo) const;
	void FormFormation(const Formation& formation);

	vector<Unit*> GetUnitsInGroup() const;
	vector<size_t> unitsInGroupIds{};
private:
	void AssignUnitsToFormation(const vector<Unit*>& units, Formation& formation) const;

	optional<Formation> desiredFormation;

	static constexpr float updateCooldown = 5.0f;
	float timeLastUpdate{};
};

