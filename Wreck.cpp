#include "precomp.h"
#include "Wreck.h"
#include "SpriteComponent.h"
#include "ColliderComponent.h"
#include "TransformComponent.h"
#include "TimeManager.h"
#include "EntityManager.h"
#include "Explosion.h"
#include "Smoke.h"
#include "MySprite.h"
#include "Scene.h"

Wreck::Wreck(Scene& scene, const Military* military, float2 position, float rotation) :
	OneOff(scene, wreckLifeSpan),
	militaryData(military->GetMilitaryData())
{
	AddComponent<TransformComponent>().SetWorldPos(position);

	uint frame = SpriteComponent::GetRotationOffset(rotation, 8);

	AddComponent<SpriteComponent>(militaryData->spriteType, -500).GetSprite()->SetFrame(frame);
	AddComponent<ColliderComponent>(0b1);
}

Wreck::~Wreck()
{

}

void Wreck::Tick()
{
	float deltaTime = TimeManager::GetDeltaTime();

	timeSinceSmoke += deltaTime;

	if (timeSinceSmoke >= smokeCooldown)
	{
		float2 myCentre = GetComponent<ColliderComponent>()->GetCentre();
		scene.entityManager->AddEntity<Smoke>(myCentre, 5.0f, 50.0f);
		timeSinceSmoke -= smokeCooldown;
	}

	OneOff::Tick();
}

void Wreck::LifeSpanReached()
{
	float2 myCentre = GetComponent<ColliderComponent>()->GetCentre();
	scene.entityManager->AddEntity<Explosion>("smoke", myCentre);
}
