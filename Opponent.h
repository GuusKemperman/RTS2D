#pragma once
#include "Entity.h"

class Army;
class Scene;
class Group;

class Opponent :
	public Entity
{
public:
	Opponent(Scene& scene, Army& army);
	~Opponent();

	void Tick() override;

private:
	void OrderToRandomPosition(Group& group) const;
	uint RandomGroupSize() const;

	Army& myArmy;

	static constexpr float tickTimestep = .5f;
	float timeSinceLastTick = tickTimestep;

	static constexpr uint maxGroupSize = 16u;
	static constexpr uint minGroupSize = 4u;

	static constexpr float minDistFromEdge = 250.0f;
	static constexpr float nonIdleGroupOrderChance = .05f;
};

