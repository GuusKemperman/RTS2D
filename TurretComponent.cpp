#include "precomp.h"
#include "TurretComponent.h"
#include "Projectile.h"
#include "Military.h"
#include "PlayerData.h"
#include "TimeManager.h"
#include "SpriteComponent.h"
#include "EntityManager.h"
#include "ColliderComponent.h"
#include "TransformComponent.h"
#include "Camera.h"
#include "SelectingComponent.h"
#include "Unit.h"
#include "Scene.h"

TurretComponent::TurretComponent(Entity* owner, const string& type) : 
	Component(owner, ComponentType::turret),
	DebugDrawable(owner->scene)
{
	turretData = Data::GetData(type);

	// This component can only be attached to entities with military as a base class.
	assert(dynamic_cast<Military*>(GetOwner()));
}

void TurretComponent::DebugDraw(Surface* target, const Camera* camera) const
{
	if (!GetOwner()->GetComponent<SelectingComponent>()->GetIsSelected()) return;

	float2 myCentre = GetOwner()->GetComponent<ColliderComponent>()->GetBoundingBox().GetCentre();

	TransformComponent* myTransform = GetOwner()->GetComponent<TransformComponent>();
	float myRotation = myTransform->GetRotation();
	float2 lineEnd1 = AngleToFloat2(NormalizeRotation(myRotation - turretData->rotationRange));
	float2 lineEnd2 = AngleToFloat2(NormalizeRotation(myRotation + turretData->rotationRange));

	Camera* cam = GetOwner()->scene.camera.get();
	int2 start = cam->WorldToVirtualScreen(myCentre);
	int2 endScrn1 = cam->WorldToVirtualScreen(myCentre + lineEnd1 * turretData->range),
		endScrn2 = cam->WorldToVirtualScreen(myCentre + lineEnd2 * turretData->range);

	const uint col = 0x80b000;
	target->Line(static_cast<float>(start.x), static_cast<float>(start.y), static_cast<float>(endScrn1.x), static_cast<float>(endScrn1.y), col);
	target->Line(static_cast<float>(start.x), static_cast<float>(start.y), static_cast<float>(endScrn2.x), static_cast<float>(endScrn2.y), col);
}

bool TurretComponent::AttemptFire(EntityId id)
{
	return false;
}

bool TurretComponent::AttemptFire(Entity* target)
{
	assert(target != nullptr);

	if (lockedOnTarget.value_or(-1) != target->GetId())
	{
		LockOnto(target->GetId());
	}

	float currentTime = TimeManager::GetTotalTimePassed();
	if (currentTime - timeLastFired < turretData->cooldown
		|| currentTime - timeLockStart < turretData->cooldown)
	{
		return false;
	}

	ColliderComponent* collider = GetOwner()->GetComponent<ColliderComponent>();

	float2 myCentre = collider->GetCentre();
	float2 targetCentre = target->GetComponent<ColliderComponent>()->GetCentre();

	if (!InSights(targetCentre))
	{
		return false;
	}

	float rotation = Float2ToAngle(targetCentre - myCentre);
	float2 offset = AngleToFloat2(rotation) * turretData->barrelLength;
	
	float2 fireFromPosition = myCentre + offset;
	Projectile& proj = GetOwner()->scene.entityManager->AddEntity<Projectile>(turretData->projectileType);
	ArmyId armyId = dynamic_cast<Military*>(GetOwner())->GetArmyId();
	proj.Fire(fireFromPosition, rotation, armyId, turretData->collisionLayer, turretData->firesWithVel, turretData->range);

	timeLastFired = currentTime;

	return true;
}

optional<Entity*> TurretComponent::PickTarget(const vector<NearbyCollider>& possibleTargets) const
{
	if (lockedOnTarget.has_value())
	{
		Entity* target = GetOwner()->scene.entityManager->TryGetEntity(lockedOnTarget.value()).value_or(nullptr);

		if (target != nullptr)
		{
			return target;
		}
	}

	Military* myOwner = dynamic_cast<Military*>(GetOwner());
	Military* target{};
	float targetDist = INFINITY;

	for (const NearbyCollider& nearbyCollider : possibleTargets)
	{
		if (nearbyCollider.distance >= targetDist)
		{
			continue;
		}

		Military* possibleTarget = dynamic_cast<Military*>(nearbyCollider.component->GetOwner());

		if (possibleTarget == nullptr ||
			possibleTarget->GetArmyId() == myOwner->GetArmyId())
		{
			continue;
		}

		if (InSights(nearbyCollider.component->GetCentre()))
		{
			target = possibleTarget;
		}
	}

	if (target)
	{
		return target;
	}
	return optional<Entity*>();
}

void TurretComponent::LockOnto(EntityId id)
{
	lockedOnTarget = id;
	timeLockStart = TimeManager::GetTotalTimePassed();
}

bool TurretComponent::InSights(const Entity* entity) const
{
	return InSights(entity->GetComponent<ColliderComponent>()->GetCentre());
}

bool TurretComponent::InSights(float2 position) const
{
	float2 myCentre = GetOwner()->GetComponent<ColliderComponent>()->GetCentre();
	float dist = Distance(position, myCentre);

	if (dist > turretData->range)
	{
		return false;
	}

	float2 deltaPos = position - myCentre;
	
	TransformComponent* myTransform = GetOwner()->GetComponent<TransformComponent>();
	float2 myForward = AngleToFloat2(myTransform->GetRotation());

	float d = dot(myForward, normalize(deltaPos));

	return abs(d - 1.0f) * 180.0f <= turretData->rotationRange;
}

TurretComponent::Data::Data(const string& type)
{
	PlayerData playerData("component.turret." + type);

	projectileType = playerData.GetVariableValue("projectiletype");
	firesWithVel = stof(playerData.GetVariableValue("fireswithvel"));
	cooldown = stof(playerData.GetVariableValue("cooldown"));
	barrelLength = stof(playerData.GetVariableValue("barrellength"));
	range = stof(playerData.GetVariableValue("range"));
	rotationRange = stof(playerData.GetVariableValue("rotationrange"));
	collisionLayer = static_cast<CollisionLayer>(stoi(playerData.GetVariableValue("collisionlayer"), 0, 2));
}
