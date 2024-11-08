#include "precomp.h"
#include "Unit.h"

#include "PerlinNoise.h"
#include "Group.h"
#include "MySprite.h"
#include "Commands.h"
#include "Camera.h"
#include "PlayerData.h"
#include "TimeManager.h"
#include "TurretComponent.h"
#include "ColliderComponent.h"
#include "TransformComponent.h"
#include "SelectingComponent.h"
#include "SpriteComponent.h"
#include "Army.h"
#include "DrawFuncs.h"
#include "Projectile.h"
#include "Scene.h"
#include "Quadtree.h"
#include "Action.h"
#include "UICanvas.h"
#include "UIMiniMap.h"

Unit::Unit(Scene& scene, Army& belongsTo, const string& type) :
	Military(scene, belongsTo, type), 
	Drawable(scene),
	unitData(Unit::Data::DataSuper::GetData(type))
{
	lastReceivedCommand = make_unique<Idle>(AggroLevel::defensive);

	timeSinceStateTick = stateTickTimeStep / static_cast<float>(GetId());

	FormGroupByMyself();

	Action* stop = Action::GetData("stop");
	actions[stop->index] = stop;
}

Unit::~Unit()
{
}

void Unit::Tick()
{
	timeSinceStateTick += TimeManager::GetDeltaTime();

	if (timeSinceStateTick >= stateTickTimeStep)
	{
		ComputeNearbyColliders();
		TickCurrentState();
		timeSinceStateTick -= stateTickTimeStep;
	}

	UpdateMovement();
	UpdateFrame();


}

void Unit::Draw(Surface* target, const Camera* camera) const
{
	ColliderComponent* collider = GetComponent<ColliderComponent>();
	float2 myCentre = collider->GetCentre();
	int2 screenPos = camera->WorldToVirtualScreen(myCentre);
	const int radius = static_cast<int>(collider->GetRadius());

	SelectingComponent* comp = GetComponent<SelectingComponent>();

	if (comp->GetIsHiglighted())
	{
		DrawFuncs::DrawCircle(target, screenPos, radius, 2, 0x8000fff0);
	}
	if (comp->GetIsSelected())
	{
		DrawFuncs::DrawCircle(target, screenPos, radius, 2, 0xff00ff00);
	}

	uint col = GetArmyId() == ArmyId::player ? 0xff : 0xff0000;


	optional<UIMiniMap*> minimap = scene.canvas->TryGetElement<UIMiniMap>("minimap");
	
	if (minimap.has_value())
	{
		minimap.value()->AddPoint(myCentre, col);
	}

#ifdef _DEBUG
	Unit::DebugDraw(target, camera);
#endif // _DEBUG

}

void Unit::DebugDraw(Surface* target, const Camera* camera) const
{
	TransformComponent* transform = GetComponent<TransformComponent>();
	ColliderComponent* collider = GetComponent<ColliderComponent>();

	float2 myCentre = collider->GetCentre();
	int2 screenPos = camera->WorldToVirtualScreen(myCentre);


	float2 desiredVelLine = screenPos + desiredVel;
	target->Line(static_cast<float>(screenPos.x), static_cast<float>(screenPos.y), static_cast<float>(desiredVelLine.x), static_cast<float>(desiredVelLine.y), 0xff);

	float2 velLine = screenPos + velocity;
	target->Line(static_cast<float>(screenPos.x), static_cast<float>(screenPos.y), static_cast<float>(velLine.x), static_cast<float>(velLine.y), 0xff0000);

	const int radius = static_cast<int>(collider->GetRadius());

	uint col{};

	switch (lastReceivedCommand->aggroLevel)
	{
	case AggroLevel::aggressive:
		col = 0xffff0000;
		break;
	case AggroLevel::passive:
		col = 0xff00ff00;
		break;
	case AggroLevel::defensive:
		col = 0xff0000ff;
		break;
	default:
		break;
	}

	DrawFuncs::DrawCircle(target, screenPos, radius / 4, col);


	if (currentState == State::movement)
	{
		MoveTo* moveTo = dynamic_cast<MoveTo*>(lastReceivedCommand.get());
		assert(moveTo != nullptr);

		if (moveTo != nullptr)
		{
			int2 lineStart = screenPos;
			float2 pos = moveTo->destination;

			int2 lineEnd = camera->WorldToVirtualScreen(pos);
			target->Line(static_cast<float>(lineStart.x), 
				static_cast<float>(lineStart.y), 
				static_cast<float>(lineEnd.x), 
				static_cast<float>(lineEnd.y), 0xa0a0a0);

			DrawFuncs::DrawCircle(target, lineEnd, radius, 2, 0xffa0a0a0);

			lineStart = lineEnd;
			

			int2 rotationLineStart = lineStart + make_int2(AngleToFloat2(moveTo->rotationAtEnd.value_or(0.0f)) * static_cast<float>(radius));
			int2 rotationLineEnd = rotationLineStart + make_int2(AngleToFloat2(moveTo->rotationAtEnd.value_or(0.0f)) * static_cast<float>(radius));
			target->Line(static_cast<float>(rotationLineStart.x), 
				static_cast<float>(rotationLineStart.y), 
				static_cast<float>(rotationLineEnd.x), 
				static_cast<float>(rotationLineEnd.y), 0xffa0a0ff);
		}
	}
}

void Unit::CommandMoveTo(const MoveTo& command)
{
	lastReceivedCommand = make_unique<MoveTo>(command);
	AttemptTransition(State::movement, true);
}

void Unit::CommandAttack(const Attack& command)
{
	FormGroupByMyself();
	lastReceivedCommand = make_unique<Attack>(command);
	AttemptTransition(State::attacking, true);
}

void Unit::CommandInvestigate(const Investigate& command)
{
	FormGroupByMyself();
	lastReceivedCommand = make_unique<Investigate>(command);
	AttemptTransition(State::investigating, true);
}

void Unit::PerformAction(const Action* action)
{
	assert(actions[action->index] == action);

	if (action == nullptr)
	{
		return;
	}

	uchar actionIndex = action->index;

	switch (actionIndex)
	{
	case 21:
		AttemptTransition(State::idle, true);
		FormGroupByMyself();
		break;
	default:
		Military::PerformAction(action);
		break;
	}
}

void Unit::TakeDamage(float amountOfDamage, const Projectile* proj)
{
	if (proj != nullptr)
	{
		float2 origin = proj->GetOrigin();
		RequestHelpNeighbours(origin);
		Investigate command(TimeManager::GetTotalTimePassed(), proj->GetOrigin());
		CommandInvestigate(command);
	}


	Military::TakeDamage(amountOfDamage);
}

void Unit::UpdateMovement()
{
	float deltaTime = TimeManager::GetDeltaTime();

	TransformComponent* myTransform = GetComponent<TransformComponent>();
	ColliderComponent* myCollider = GetComponent<ColliderComponent>();

	float currentRotation = myTransform->GetRotation();
	float rotateTowards = desiredRotation.value_or(sqrLength(desiredVel) != 0.0f ? Float2ToAngle(desiredVel) : currentRotation);

	myTransform->RotateTowards(rotateTowards, unitData->maxTurn * deltaTime);
	currentRotation = myTransform->GetRotation();

	// The desired currentRotation trumps the desired vel, brake when the desired currentRotation has a value.
	if (desiredRotation.has_value())
	{
		desiredVel = 0.0f;
	}

	float2 steeringDirection = desiredVel - velocity;
	float2 steeringForce = length(steeringDirection) > unitData->maxForce ? normalize(steeringDirection) * unitData->maxForce : steeringDirection;
	float2 acceleration = steeringForce / unitData->mass;

	velocity += steeringForce;

	if (length(velocity) > unitData->maxSpeed)
	{
		velocity = normalize(velocity) * unitData->maxSpeed;
	}

	//Should move slower when lots of turning needs to be done.
	float amountOfTurningLeft = DistanceBetweenAngles(rotateTowards, currentRotation);

	if (abs(amountOfTurningLeft) >= unitData->maxTurn)
	{
		// Braking
		velocity -= steeringForce;

		if (length(velocity) != 0.0f)
		{
			velocity = normalize(velocity) * max(length(velocity) - (unitData->maxForce / unitData->mass), 0.0f);

		}
	}

	velocity = AngleToFloat2(currentRotation) * length(velocity);

	float2 worldPos = myTransform->GetWorldPos();
	float2 proposedPos = worldPos + velocity * deltaTime;

	float2 topLeft(min(worldPos.x, proposedPos.x), min(worldPos.y, proposedPos.y));
	float2 bottomRight = float2(max(worldPos.x, proposedPos.x), max(worldPos.y, proposedPos.y)) + myCollider->GetBoundingBox().GetSize();

	BoundingBox checkingArea(BoundingBox(topLeft, bottomRight));
	vector<ColliderComponent*> possibleCollisions = myCollider->CollidersInArea(checkingArea, true);


	if (CanMoveTo(proposedPos, myCollider, possibleCollisions))
	{
		myTransform->SetWorldPos(proposedPos);
		return;
	}

	// Sliding
	if (CanMoveTo({ proposedPos.x, worldPos.y }, myCollider, possibleCollisions))
	{
		myTransform->SetWorldPos({ proposedPos.x, worldPos.y });
		velocity.y = 0.0f;
		return;
	}
	if (CanMoveTo({ worldPos.x, proposedPos.y }, myCollider, possibleCollisions))
	{
		myTransform->SetWorldPos({ worldPos.x, proposedPos.y });
		velocity.x = 0.0f;
		return;
	}
}

float2 Unit::Avoidance() const
{
	ColliderComponent* myCollider = GetComponent<ColliderComponent>();

	// Cull the objects outside of the avoidanceCircle.
	float2 myCentre = myCollider->GetCentre();
	float2 avoidanceForce{};
	float myRadius = myCollider->GetRadius();

	for (const NearbyCollider& nearbyCollider : nearbyColliders)
	{
		if ((nearbyCollider.component->GetLayer() & myCollider->GetLayer()) == 0)
		{
			continue;
		}

		float spacing = nearbyCollider.distance - myRadius - nearbyCollider.component->GetRadius();

		if (spacing > desiredSpacing)
		{
			continue;
		}

		float2 closestColliderCentre = nearbyCollider.component->GetCentre();
		float2 deltaPos = myCentre - closestColliderCentre;

		float avoidanceStrength = clamp((1.0f - (spacing / desiredSpacing)), 0.0f, 1.0f);

		avoidanceForce += normalize(deltaPos) * avoidanceStrength * unitData->maxSpeed;
	}

	return avoidanceForce;
}

float2 Unit::Seek(float2 towardPosition) const
{
	float2 myCentre = GetComponent<ColliderComponent>()->GetCentre();

	float2 deltaPos = towardPosition - myCentre;

	return deltaPos == float2{ 0.0f } ? 0.0f : normalize(deltaPos)* unitData->maxSpeed;
}

float2 Unit::FollowPath(list<float2>& path) const
{
	if (path.empty())
	{
		return { 0.0f, 0.0f };
	}

	float2 nextPathPoint = path.front();
	float2 arrivalVel = Arrival(nextPathPoint);

	if (arrivalVel == float2{ 0.0f })
	{
		path.pop_front();
	}

	return arrivalVel;
}

float2 Unit::Arrival(float2 arriveAt) const
{
	float2 myCentre = GetComponent<ColliderComponent>()->GetCentre();

	float2 targetOffset = arriveAt - myCentre;
	float distToTarget = length(targetOffset);

	if (distToTarget == 0.0f)
	{
		return { 0.0f };
	}

	float deacceleration = (unitData->maxForce / unitData->mass);
	float timeToBreak = unitData->maxSpeed / deacceleration + stateTickTimeStep;
	float slowingDist = unitData->maxSpeed * timeToBreak - (deacceleration / 2.0f) * (timeToBreak * timeToBreak);

	float rampedSpeed = unitData->maxSpeed * (distToTarget / slowingDist);
	float clippedSpeed = min(rampedSpeed, unitData->maxSpeed);

	float2 vel = targetOffset * (clippedSpeed / distToTarget);

	return sqrLength(vel) < 0.01f ? 0.0f : vel;
}

float2 Unit::Wander() const
{
	static const siv::PerlinNoise perlin{ 0 };

	double time = static_cast<double>(TimeManager::GetTotalTimePassed());
	double samplePoint = static_cast<double>(GetId()) * 15059223599.0 + time;
	
	float rotationNoise = static_cast<float>(perlin.noise1D_01(samplePoint * wanderAmplitude));
	float magnitudeNoise = static_cast<float>(perlin.noise1D_01(samplePoint * wanderAmplitude + 312412415.0));

	return AngleToFloat2(rotationNoise * 360.0f) * magnitudeNoise * unitData->maxSpeed;
}

void Unit::UpdateFrame()
{
	float rotation = GetComponent<TransformComponent>()->GetRotation();
	uint rotationOffset = SpriteComponent::GetRotationOffset(rotation, 8);

	GetComponent<SpriteComponent>()->GetSprite()->SetFrame(rotationOffset + static_cast<int>(GetArmyId()) * 8);
}

void Unit::TickCurrentState()
{
	transitionsThisFrame = 0;
	desiredVel = 0.0f;
	desiredRotation.reset();

	State stateBeforeTick = currentState;

	switch (currentState)
	{
		case State::idle:
		{
			IdleStateTick();
			break;
		}
		case State::movement:
		{
			MovementStateTick();
			break;
		}
		case State::attacking:
		{
			AttackStateTick();
			break;
		}
		case State::investigating:
		{
			InvestigateTick();
			break;
		}
		default:
		{
			assert(false);
			break;
		}
	}

	if (stateBeforeTick != currentState)
	{
		TickCurrentState();
	}
}

bool Unit::AttemptTransition(State toState, bool condition)
{
	if (condition)
	{
		transitionsThisFrame++;
		assert(transitionsThisFrame < maxTransitionsPerFrame && "Entered infinite loop with transitions");
		currentState = toState;
	}
	return condition;
}

void Unit::IdleStateTick()
{
	if (lastReceivedCommand->aggroLevel == AggroLevel::aggressive)
	{
		optional<Entity*> enemyInSight = GetComponent<TurretComponent>()->PickTarget(nearbyColliders);

		if (AttemptTransition(State::attacking, enemyInSight.has_value()))
		{
			return;
		}
	}
}

void Unit::MovementStateTick()
{
	MoveTo* moveTo = dynamic_cast<MoveTo*>(lastReceivedCommand.get());

	if (AttemptTransition(State::idle, moveTo == nullptr))
	{
		return;
	}

	if (moveTo->aggroLevel == AggroLevel::aggressive)
	{
		optional<Entity*> enemyInSight = GetComponent<TurretComponent>()->PickTarget(nearbyColliders);

		if (AttemptTransition(State::attacking, enemyInSight.has_value()))
		{
			return;
		}
	}

	float2 destination = moveTo->destination;
	float2 arrivalVel = Arrival(destination);

	TransformComponent* myTransform = GetComponent<TransformComponent>();
	float rotatingLeft = DistanceBetweenAngles(myTransform->GetRotation(), moveTo->rotationAtEnd.value_or(0.0f));

	float arrivalStrength = length(arrivalVel);

	if (AttemptTransition(State::idle, arrivalStrength <= 0.05f && rotatingLeft == 0.0f))
	{
		return;
	}

	if (arrivalStrength == 0.0f)
	{
		desiredRotation = moveTo->rotationAtEnd;
		return;
	}

	float2 avoidanceVel = Avoidance();

	float avoidanceStrength = length(avoidanceVel);

	if (avoidanceStrength == 0.0f)
	{
		desiredVel = arrivalVel;
		return;
	}
	else
	{
		arrivalStrength -= avoidanceStrength;

		desiredVel = normalize(arrivalVel) * arrivalStrength + normalize(avoidanceVel) * avoidanceStrength;
	}
}

void Unit::AttackStateTick()
{
	// If targetId has been killed and no other targets in sights.

	TurretComponent* myTurret = GetComponent<TurretComponent>();
	assert(myTurret != nullptr);

	Attack* command = dynamic_cast<Attack*>(lastReceivedCommand.get());
	optional<Entity*> target = scene.entityManager->TryGetEntity(command != nullptr ? command->target.value_or(-1) : -1);

	// If the targetId has been killed, pick a new target.
	if (!target.has_value())
	{
		if (command != nullptr)
		{
			command->target.reset();
		}
		target = myTurret->PickTarget(nearbyColliders);
	}

	// If the turret could not pick a new target, go to idle.
	if (AttemptTransition(State::idle, !target.has_value()))
	{
		return;
	}

	myTurret->AttemptFire(target.value());

	// Move towards target if too far away
	float2 myCentre = GetComponent<ColliderComponent>()->GetCentre();
	float2 targetCentre = target.value()->GetComponent<ColliderComponent>()->GetCentre();

	float2 deltaPos = targetCentre - myCentre;

	float sqrdDistanceToTarget = sqrLength(deltaPos);
	float sqrdMaxDesiredDist = sqrf(myTurret->GetRange());
	float sqrdMinDesiredDist = sqrf(myTurret->GetRange() * attackingApproachTreshold);

	float scalar = clamp((sqrdDistanceToTarget - sqrdMinDesiredDist) / (sqrdMaxDesiredDist - sqrdMinDesiredDist), 0.0f, 1.0f);

	if (scalar == 0.0f)
	{
		desiredRotation = Float2ToAngle(deltaPos);
		return;
	}

	float2 seek = Seek(targetCentre) * scalar;

	float2 avoidance = Avoidance();
	float avoidanceLength = length(avoidance);
	float seekLength = length(seek);

	if (avoidanceLength == 0.0f || seekLength == 0.0f)
	{
		desiredVel = seek;
		return;
	}

	float seekStrength = seekLength - avoidanceLength;
	desiredVel = (seek / seekLength) * seekStrength + avoidance;
}

void Unit::InvestigateTick()
{
	Investigate* command = dynamic_cast<Investigate*>(lastReceivedCommand.get());
	assert(command != nullptr);

	float currentTime = TimeManager::GetTotalTimePassed();

	if (AttemptTransition(State::idle, command->timeInvestigationStart + investigationDuration <= currentTime))
	{
		return;
	}

	TurretComponent* turret = GetComponent<TurretComponent>();
	assert(turret != nullptr);

	optional<Entity*> target = turret->PickTarget(nearbyColliders);

	if (target.has_value())
	{
		CommandAttack({ target.value()->GetId() });
		return;
	}

	float2 avoidance = Avoidance();
	float2 investigation = (Arrival(command->position) + Wander()) * .5f;

	float avoidLength = length(avoidance);
	float investigateLength = length(investigation);

	if (avoidLength == 0.0f || investigateLength == 0.0f)
	{
		desiredVel = investigation;
		return;
	}

	float avoidanceWeight = avoidLength;
	float investigateWeight = investigateLength - avoidLength;

	desiredVel = (avoidance / avoidLength) * avoidanceWeight + (investigation / investigateLength) * investigateWeight;
}

bool Unit::CanMoveTo(float2 position, const ColliderComponent* myCollider, const vector<ColliderComponent*>& possibleCollisions) const
{
	BoundingBox myBBWhenAtPos(position, myCollider->GetBoundingBox().GetSize());

	if (!scene.quadTree->GetMaxBounds().Contains(myBBWhenAtPos))
	{
		return false;
	}

	for (const ColliderComponent* nearbyCollider : possibleCollisions)
	{
		const BoundingBox& otherBB = nearbyCollider->GetBoundingBox();
		if(!myBBWhenAtPos.Intersects(otherBB))
		{
			continue;
		}

		const Unit* owner = dynamic_cast<const Unit*>(nearbyCollider->GetOwner());
		if (owner == nullptr)
		{
			return false;
		}

		float minDist = myCollider->GetRadius() + nearbyCollider->GetRadius();
		// For other units, use circle collision
		if (SqrdDistance(otherBB.GetCentre(), myBBWhenAtPos.GetCentre()) < minDist * minDist)
		{
			return false;
		}

	}

	return true;
}

void Unit::RequestHelpNeighbours(float2 atPosition)
{
	// This function is called outside of the tick function, so the neighbours may be invalid.
	ComputeNearbyColliders();

	Investigate command(TimeManager::GetTotalTimePassed(), atPosition);
	for (const NearbyCollider& nearbyCollider : nearbyColliders)
	{
		if (nearbyCollider.distance > communicationRadius)
		{
			continue;
		}

		Unit* unit = dynamic_cast<Unit*>(nearbyCollider.component->GetOwner());
		
		if (unit != nullptr 
			&& unit->GetArmyId() == GetArmyId() 
			&& unit->GetState() != State::investigating
			&& unit->lastReceivedCommand->aggroLevel != AggroLevel::passive
			&& unit->GetComponent<TurretComponent>() != nullptr)
		{
			unit->CommandInvestigate(command);
		}
	}
}

void Unit::AlertNearbyAllies(EntityId ofTarget) const
{
	Attack command(ofTarget);
	for (const NearbyCollider& nearbyCollider : nearbyColliders)
	{
		if (nearbyCollider.distance > communicationRadius)
		{
			continue;
		}

		Unit* unit = dynamic_cast<Unit*>(nearbyCollider.component->GetOwner());

		if (unit != nullptr
			&& unit->GetArmyId() == GetArmyId()
			&& unit->lastReceivedCommand->aggroLevel == AggroLevel::aggressive)
		{
			unit->CommandAttack(command);
		}
	}
}

void Unit::FormGroupByMyself()
{
	partOfGroup = scene.entityManager->AddEntity<Group>(vector<EntityId>{ GetId() }, false).GetId();
}

void Unit::ComputeNearbyColliders()
{
	float nearbyTreshold = GetComponent<TurretComponent>()->GetRange();
	nearbyColliders = GetComponent<ColliderComponent>()->CollidersInRange(nearbyTreshold, false);
}

Unit::Data::Data(const string& type)
{
	PlayerData playerData("entity.military." + type);

	mass = stof(playerData.GetVariableValue("mass"));	
	maxTurn = stof(playerData.GetVariableValue("maxturn"));
	maxForce = stof(playerData.GetVariableValue("maxforce"));
	maxSpeed = stof(playerData.GetVariableValue("maxspeed"));
}