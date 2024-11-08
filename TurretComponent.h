#pragma once
#include "Component.h"
#include "DebugDrawable.h"
#include "DataSuper.h"
#include "NearbyCollider.h"

class ColliderComponent;

class TurretComponent : 
	public Component, 
	public DebugDrawable
{
public:
	TurretComponent(Entity* owner, const string& type);

	void DebugDraw(Surface* target, const Camera* camera) const override;

	optional<Entity*> PickTarget(const vector<NearbyCollider>& possibleTargets) const;
	
	bool InSights(const Entity* entity) const;
	bool InSights(float2 position) const;

	bool AttemptFire(EntityId id);
	bool AttemptFire(Entity* target);

	class Data : public DataSuper<Data>
	{
		friend DataSuper;
		Data(const string& type);

	public:
		string projectileType{};
		float firesWithVel{};
		float cooldown{};
		float barrelLength{};
		float range{};
		float rotationRange{};
		CollisionLayer collisionLayer{};
	};

	inline float GetRange() const { return turretData->range; }

private:
	void LockOnto(EntityId id);


	optional<EntityId> lockedOnTarget{};
	float timeLockStart{};

	const Data* turretData{};
	float timeLastFired{};
};

