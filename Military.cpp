#include "precomp.h"
#include "Military.h"
#include "TurretComponent.h"
#include "PlayerData.h"
#include "TransformComponent.h"
#include "SpriteComponent.h"
#include "SelectingComponent.h"
#include "EntityManager.h"
#include "Wreck.h"
#include "Explosion.h"
#include "ColliderComponent.h"
#include "Army.h"

Military::Military(Scene& scene, Army& belongsTo, string type) :
	Entity(scene),
	partOfArmy( belongsTo ), 
	militaryData(Military::Data::DataSuper::GetData(type))
{
	AddComponent<TransformComponent>();
	AddComponent<SpriteComponent>(militaryData->spriteType);
	AddComponent<ColliderComponent>(militaryData->collisionLayer);

	AddComponent<TurretComponent>(militaryData->turretType);

	AddComponent<SelectingComponent>();

	health = militaryData->maxHealth;
}

Military::~Military()
{

}

void Military::Tick()
{

}

void Military::TakeDamage(float amountOfDamage, const Projectile* proj)
{
	if (health == 0.0f)
	{
		return;
	}

	health = clamp(health - amountOfDamage, 0.0f, militaryData->maxHealth);
	if (health == 0.0f)
	{
		Destroy();
	}
}

void Military::Destroy()
{
	EntityManager& inst = *scene.entityManager;
	TransformComponent* transform = GetComponent<TransformComponent>();

	float2 myWorldPos = transform->GetWorldPos();
	float myRotation = transform->GetRotation();
	float2 myCentre = GetComponent<ColliderComponent>()->GetCentre();

	inst.AddEntity<Explosion>(militaryData->explosionType, myCentre);
	inst.AddEntity<Wreck>(this, myWorldPos, myRotation);
	inst.RemoveEntity(GetId());
}

const ArmyId Military::GetArmyId() const
{
	return partOfArmy.armyId;
}

Military::Data::Data(const string& type)
{
	PlayerData playerData("entity.military." + type);

	maxHealth = stof(playerData.GetVariableValue("maxhealth"));
	
	spriteType = playerData.GetVariableValue("spritetype");
	explosionType = playerData.GetVariableValue("explosiontype");
	turretType = playerData.GetVariableValue("turrettype");

	collisionLayer = static_cast<CollisionLayer>(stoi(playerData.GetVariableValue("collisionlayer"), 0, 2));

}
